{
  'includes': [ 'common.gypi' ],
  'targets': [
    { 'target_name': '01_alarm'        , 'sources': [ 'src/01_alarm.c' ] }        ,
    { 'target_name': '02_alarm_fork'   , 'sources': [ 'src/02_alarm_fork.c' ] }   ,
    { 'target_name': '03_alarm_thread' , 'sources': [ 'src/03_alarm_thread.c' ] } ,
    { 'target_name': '04_thread_error' , 'sources': [ 'src/04_thread_error.c' ] } ,
    { 'target_name': '05_lifecycle'    , 'sources': [ 'src/05_lifecycle.c' ] }    ,
  ]
}
