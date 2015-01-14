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
    { 'target_name': 'alarm', 'sources': [ 'src/alarm.c' ] }
  ]
}
