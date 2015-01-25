{
  'includes': [ 'common.gypi' ],
  'targets': [
    { 'target_name': '01_alarm'        , 'sources': [ 'src/01_alarm.c' ] }        ,
    { 'target_name': '02_alarm_fork'   , 'sources': [ 'src/02_alarm_fork.c' ] }   ,
    { 'target_name': '03_alarm_thread' , 'sources': [ 'src/03_alarm_thread.c' ] } ,
  ]
}
