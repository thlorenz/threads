{
  'includes': [ 'common.gypi' ],
  'targets': [
    { 'target_name': '01_alarm'         , 'sources': [ 'src/01_alarm.c' ] }         ,
    { 'target_name': '02_alarm_fork'    , 'sources': [ 'src/02_alarm_fork.c' ] }    ,
    { 'target_name': '03_alarm_thread'  , 'sources': [ 'src/03_alarm_thread.c' ] }  ,
    { 'target_name': '04_thread_error'  , 'sources': [ 'src/04_thread_error.c' ] }  ,
    { 'target_name': '05_lifecycle'     , 'sources': [ 'src/05_lifecycle.c' ] }     ,
    { 'target_name': '06_mutex_dynamic' , 'sources': [ 'src/06_mutex_dynamic.c' ] } ,
    { 'target_name': '07_alarm_mutex'   , 'sources': [ 'src/07_alarm_mutex.c' ] }   ,
    { 'target_name': '08_try_lock'      , 'sources': [ 'src/08_try_lock.c' ] }      ,
    { 'target_name': '09_backoff'       , 'sources': [ 'src/09_backoff.c' ] }       ,
    { 'target_name': '10_cond'          , 'sources': [ 'src/10_cond.c' ] }          ,
    { 'target_name': '11_alarm_cond'    , 'sources': [ 'src/11_alarm_cond.c' ] }    ,
  ]
}
