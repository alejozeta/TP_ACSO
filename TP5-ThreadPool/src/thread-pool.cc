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
        wts[i].available = true;
        wts[i].ts = thread([this, i]() {
            worker(i);
        });
    }

    // creo hilo dispatcher, despierta workers disponibles
    dt = thread([this]() {
        dispatcher();
    });
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

}

void ThreadPool::dispatcher() {
    while (true) {
        function<void(void)> nextTask;

        {
            unique_lock<mutex> lock(queueLock);
            queueCV.wait(lock, [this]() {
                return done || !taskQueue.empty();
            });

            if (done && taskQueue.empty()) break;

            nextTask = taskQueue.front();
            taskQueue.pop();
        }

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
                    goto ready; //salto a et ready ya que hay worker disponible
                }
            }
        }

    ready:
        wts[workerID].sema.signal();
    }
}

void ThreadPool::worker(int id) {
    worker_t& self = wts[id];

    while (true) {
        self.sema.wait();  // espera que dispatcher lo despierte

        if (done) break;  // en shutdown: salir

        // Ejecutar el thunk
        self.thunk();  // ejecuta la función asignada
        self.thunk = nullptr;
        
        { //Después de ejecutar la tarea, marcar este worker como disponible
            lock_guard<mutex> lk(self.mtx);
            self.available = true;
        }

        {
            lock_guard<mutex> lk(waitLock);
            pendingTasks--;
            if (pendingTasks == 0)
                waitCV.notify_all();
        }
    }
    this_thread::yield();
}


void ThreadPool::wait() {
    unique_lock<mutex> lk(waitLock);
    waitCV.wait(lk, [this]() {
        return pendingTasks == 0;
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