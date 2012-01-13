#ifndef BMUTEX_HPP
#define BMUTEX_HPP

// Use Window or Posix
#ifdef WINDOWS
     #include <windows.h>
#else
     #ifndef POSIX
          #warning POSIX will be used (but you did not define it)
     #endif
     #include <pthread.h>
#endif
 
/**
* @author Berenger
* @version 0.5
* @date February 15 2010
* @file BMutex.hpp
* @package Package-OS specific (POSS)
* @brief Mutex
*
*
* This class represent a simple way to use mutex
*
* @example BMutex mut;
* @example mut.lock();      // lock
* @example ...
* @example mut.islocked();  // fast look up
* @example ...
* @example mut.unlock();    // unlock
* @example mut.tryLock();   // try lock
*
* Ressources : http://www.codeproject.com/KB/threads/thread_class.aspx
*
* @must You may have to change this class if you are not on Windows
* @must or Posix OS
*
* All methods may be inline by the compiler
* @copyright Brainable.Net
*/
 
class BMutex{
private:
 
#ifdef WINDOWS
     CRITICAL_SECTION _mutex; /**< Window mutex */
#else
     pthread_mutex_t _mutex; /**< posix mutex */
#endif
 
     bool _locked;           /**< Fast locked look up used for copying */
 
     void init(){
     #ifdef WINDOWS
          InitializeCriticalSection(&_mutex);
     #else
          pthread_mutexattr_t attr;
          pthread_mutexattr_init(&attr);
          pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
          pthread_mutex_init(&_mutex,&attr);
          pthread_mutexattr_destroy(&attr);
     #endif
          _locked = false;
     }
 
public:
 
     /**
     * @brief Construct a BMutex
     * @brief Posix and Win mutex
     */
     BMutex(){
          init();
     }
     /**
     * @brief Copy Constructor a mutex (copy the locked state only)
     * @param Based mutex
     *
     */
     BMutex( const BMutex &in_mutex ) {
          init();
 
          if(in_mutex._locked && !_locked) lock();
          else if(!in_mutex._locked && _locked) unlock();
     }
 
     /**
     * @brief Copy a mutex (copy the locked state only)
     * @param Based mutex
     * @return Current mutex
     */
     BMutex& operator=(const BMutex &in_mutex) {
          if(in_mutex._locked && !_locked) lock();
          else if(!in_mutex._locked && _locked) unlock();
          return *this;
     }
 
     /**
     * @brief Destructor
     */
     virtual ~BMutex(){
     #ifdef WINDOWS
          DeleteCriticalSection(&_mutex);
     #else
          pthread_mutex_unlock(&_mutex);
          pthread_mutex_destroy(&_mutex);
     #endif
     }
 
     /**
     * @brief lock a mutex
     * @return WIN true
     * @return POSIX true if success
     */
     bool lock(){
          _locked = true;
     #ifdef WINDOWS
          EnterCriticalSection(&_mutex);
          return true;
     #else
          return pthread_mutex_lock(&_mutex) == 0;
     #endif
     }
 
     /**
     * @brief lock a mutex
     * @return true if success else false (if busy or error)
     */
     bool tryLock(){
          _locked = true;
     #ifdef WINDOWS
          return TryEnterCriticalSection(&_mutex);
     #else
          return pthread_mutex_trylock(&_mutex) == 0;
     #endif
     }
 
     /**
     * @brief unlock a mutex
     * @return WIN true in every cases
     * @return POSIX true if success
     */
     bool unlock(){
          _locked = false;
     #ifdef WINDOWS
          LeaveCriticalSection(&_mutex);
          return true;
     #else
          return pthread_mutex_unlock(&_mutex) == 0;
     #endif
     }
 
     /**
     * @brief Fast locked look up
     * @return true if locked else false
     * This methode use the fast look up variable but still CONST
     * if you want to test perfectly do :
     * if(myMutex.tryLock()){
     *      myMutex.unlock();
     *      // I am sure that the mutex is not locked
     * }
     * else{
     *      // The mutex is locked
     * }
     */
     bool isLocked() const{
          return _locked;
     }
 
};

class BMutexLock
{
    BMutex& m_mutex;
    
public:
    BMutexLock( BMutex& mutex ) 
    : m_mutex(mutex) 
    { 
        m_mutex.lock(); 
    }
    
    ~BMutexLock() 
    { 
        m_mutex.unlock();
    }
};
 
#endif