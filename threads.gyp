{
  'includes': [ 'common.gypi' ],
  'targets': [
    { 'target_name': '01_alarm'         , 'sources': [ 'src/prog-with-posix-threads/01_alarm.c' ] }         ,
    { 'target_name': '02_alarm_fork'    , 'sources': [ 'src/prog-with-posix-threads/02_alarm_fork.c' ] }    ,
    { 'target_name': '03_alarm_thread'  , 'sources': [ 'src/prog-with-posix-threads/03_alarm_thread.c' ] }  ,
    { 'target_name': '04_thread_error'  , 'sources': [ 'src/prog-with-posix-threads/04_thread_error.c' ] }  ,
    { 'target_name': '05_lifecycle'     , 'sources': [ 'src/prog-with-posix-threads/05_lifecycle.c' ] }     ,
    { 'target_name': '06_mutex_dynamic' , 'sources': [ 'src/prog-with-posix-threads/06_mutex_dynamic.c' ] } ,
    { 'target_name': '07_alarm_mutex'   , 'sources': [ 'src/prog-with-posix-threads/07_alarm_mutex.c' ] }   ,
    { 'target_name': '08_try_lock'      , 'sources': [ 'src/prog-with-posix-threads/08_try_lock.c' ] }      ,
    { 'target_name': '09_backoff'       , 'sources': [ 'src/prog-with-posix-threads/09_backoff.c' ] }       ,
    { 'target_name': '10_cond'          , 'sources': [ 'src/prog-with-posix-threads/10_cond.c' ] }          ,
    { 'target_name': '11_alarm_cond'    , 'sources': [ 'src/prog-with-posix-threads/11_alarm_cond.c' ] }    ,
    { 'target_name': 'sigprof'          , 'sources': [ 'src/sigprof.cc' ] }                                 ,
  ]
}
