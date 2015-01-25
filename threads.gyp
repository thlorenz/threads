{
  'target_defaults': {
    'conditions': [
      ['OS != "win"', {
        'conditions': [
          ['OS=="solaris"', {
            'cflags': [ '-pthreads' ],
          }],
          ['OS not in "solaris android"', {
            'cflags': [ '-pthread' ],
          }],
        ],
      }],
    ],
    'include_dirs': [ 'src' ],
    'sources': [ 'src/errors.h' ],
    'xcode_settings': {
      'MACOSX_DEPLOYMENT_TARGET': '10.7',
    },
    'type' : 'executable',
  },
  'targets': [
    { 'target_name': '01_alarm'      , 'sources': [ 'src/01_alarm.c' ] },
    { 'target_name': '02_alarm_fork' , 'sources': [ 'src/02_alarm_fork.c' ] },
  ]
}
