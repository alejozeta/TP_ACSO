/**
 * File: thread-pool.cc
 * --------------------
 * Presents the implementation of the ThreadPool class.
 */

#include "thread-pool.h"
using namespace std;

// el destructor tiene que esperar a que terminen todos los hilos

ThreadPool::ThreadPool(size_t numThreads) : wts(numThreads), done(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        // Construcción directa del semáforo usando placement new
        new (&wts[i].sema) Semaphore(0);

        wts[i].available = true; // Indica que el worker i está inicialmente libre para recibir tareas

        // Crear el thread después de que todo el resto esté listo
        wts[i].ts = thread([this, i]() {
            worker(i);
        });
    }

    // creo hilo dispatcher, despierta workers disponibles
    dt = thread([this]() {
        dispatcher();
    });
    dispatcherSignal = new Semaphore(0); // Inicializar el semáforo del dispatcher
}


void ThreadPool::schedule(const function<void(void)>& thunk) {
    if (!thunk) {throw invalid_argument("Cannot schedule a null function");}
    if (done) { throw runtime_error("Cannot schedule tasks on a destroyed ThreadPool");}
    {
        lock_guard<mutex> lock(queueLock);
        taskQueue.push(thunk);
        pendingTasks++;
    }
    queueCV.notify_one();  // Notificar al dispatcher que hay una nueva tarea

    dispatcherSignal->signal(); // Despertar al dispatcher si está esperando

}

void ThreadPool::dispatcher() {
    while (true) {
        function<void(void)> nextTask;
        dispatcherSignal->wait(); // Espera a que haya una tarea disponible
        if (done ) break;

        // Esperar hasta que haya un worker disponible
        size_t workerID = -1;
        while (true) {
            for (size_t i = 0; i < wts.size(); ++i) {
                unique_lock<mutex> lk(wts[i].mtx);
                if (wts[i].available) {
                    // Marcar worker como ocupado y pasarle la tarea
                    wts[i].available = false;
                    wts[i].thunk = nextTask;
                    workerID = i;
                    break;
                }
            }
            // No hay workers disponibles, esperar a que haya una tarea
            unique_lock<mutex> lk(queueLock);
            queueCV.wait(lk, [this]() {
                return !taskQueue.empty() || done;
            });
        }

        if (!(workerID ==-1)){
            function<void(void)> task;
            {
                lock_guard<mutex> lk(queueLock);
                if (!taskQueue.empty()) {
                    task = taskQueue.front();
                    taskQueue.pop();
                }
            }
            {
                lock_guard<mutex> lk(wts[workerID].mtx);
                wts[workerID].thunk = task; // Asignar la tarea al worker
            }
            wts[workerID].sema.signal(); // Despertar al worker para que
        }
    }
}    

void ThreadPool::worker(int id) {
    worker_t& self = wts[id];

    while (true) {
        self.sema.wait();  // espera que dispatcher lo despierte

        if (done) break;  // en shutdown: salir

        // Copiar la tarea a ejecutar
        function<void(void)> task;
        {
            lock_guard<mutex> lk(self.mtx);
            task = self.thunk;
        }

        // Ejecutar la tarea
        if (task) {
            task();  // ejecutar la tarea
        }
        {
            lock_guard<mutex> lk(self.mtx);
            self.available = true;  // marcar como disponible
            pendingTasks--;         // decrementar el contador de tareas pendientes
        }
        waitCV.notify_all();  // Notificar al dispatcher que hay un worker disponible

    }
}

void ThreadPool::wait() {
    unique_lock<mutex> lock(queueLock);
    queueCV.wait(lock, [this]() {
        return taskQueue.empty() && pendingTasks.load() == 0;
    });

}


ThreadPool::~ThreadPool() {
    wait(); // asegurarse de que no quedan tareas

    {
        lock_guard<mutex> lock(queueLock);
        done = true;
        queueCV.notify_all();  // despertar al dispatcher si estaba esperando
    }

    // Señalamos a todos los workers para que terminen
    for (auto& w : wts) {
        w.sema.signal(); // por si están bloqueados en wait()
    }

    // Join de todos los hilos de trabajadores
    for (auto& w : wts) {
        if (w.ts.joinable())
            w.ts.join();
    }

    // Join del dispatcher
    if (dt.joinable())
        dt.join();
}
