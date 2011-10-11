/*
 *    The main ThreadClass
 *    Copyright (C) 2011  Lukas Zurschmiede <l.zurschmiede@delightsoftware.com>
 * 
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include "thread.h"

namespace amods {
  Thread::Thread() {
    pthread_attr_init(&p_attr);
    pthread_attr_setdetachstate(&p_attr, PTHREAD_CREATE_JOINABLE);
  }
  Thread::~Thread() {
    pthread_attr_destroy(&p_attr);
  }
  
  /**
   * Statring the Thread and return the Code
   * 
   * @arg void* arg Any Parameters you need in the object later
   * @return int 0 on success, otherwise the ErrorNumber of pthread_create
   * @public
   */
  int Thread::Start(void* arg) {
    Arg(arg);
    int code = pthread_create(&p_thread, NULL, Thread::EntryPoint, (void*)this);
    return code;
  }
  
  /**
   * The main EntryPoint for the Thread-Function pthread_create
   * 
   * @arg AMoDS* Pointer to this instance
   * @protected
   * @static
   */
  void* Thread::EntryPoint(void* this_pt) {
    Thread* pt = (Thread*)this_pt;
    pt->Run( pt->Arg());
  }
  
  /**
   * Called after the Thread is created and running
   * 
   * @arg void* arg Any Parameters you need in the object later
   * @return 
   * @protected
   */
  int Thread::Run(void* arg) {
    Setup();
    Execute(arg);
  }
  
  /**
   * Wait until the Thread ends
   * 
   * @return int Exit Code
   * @public
   */
  int Thread::WaitUntilEnd() {
    int rc;
    void* status;
    rc = pthread_join(p_thread, &status);
    return rc;
  }
  
  /**
   * Called before the Execute - to setup anything
   * This function may be overriden
   * @virtual
   * @protected
   */
  void Thread::Setup() {}
  
  /**
   * Called by the Thread just after Setup - Execute all Code here
   * This function must be overriden
   * @virtual
   * @protected
   */
  void Thread::Execute(void* arg) { }
  
}
