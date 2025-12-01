{
  'variables': {
    'module_name': 'NodeQuickfix',
    'module_path': './lib/binding/Release/'
  },
  'targets': [
    {
      'target_name': 'NodeQuickfix',
      'sources': [
        'src/Threading.h',
        'src/Dispatcher.h',
      	'src/FixCredentials.h',
      	'src/FixEvent.h',
      	'src/FixEventQueue.h',
      	'src/FixSession.h',
      	'src/FixSession.cpp',
      	'src/FixLogon.h',
      	'src/FixLoginProvider.h',
      	'src/FixLoginProvider.cpp',
      	'src/FixLoginResponse.h',
      	'src/FixLoginResponse.cpp',
      	'src/FixAcceptor.h',
      	'src/FixAcceptor.cpp',
      	'src/FixAcceptorStartWorker.h',
      	'src/FixAcceptorStartWorker.cpp',
      	'src/FixAcceptorStopWorker.h',
      	'src/FixAcceptorStopWorker.cpp',
      	'src/FixApplication.h',
      	'src/FixApplication.cpp',
      	'src/FixConnection.h',
      	'src/FixConnection.cpp',
      	'src/FixInitiator.h',
      	'src/FixInitiator.cpp',
      	'src/FixInitiatorStartWorker.h',
      	'src/FixInitiatorStartWorker.cpp',
      	'src/FixInitiatorStopWorker.h',
      	'src/FixInitiatorStopWorker.cpp',
      	'src/FixMessageUtil.h',
      	'src/FixSendWorker.h',
      	'src/FixSendWorker.cpp',
      	'src/node_quickfix.cpp'
      ],
      'link_settings': {
        'libraries': [
          '-L/usr/lib',
          '-L/usr/local/lib',
          '-lquickfix',
          '-lpthread',
          '-lxml2',
          '-lz'
        ]
      },
      'include_dirs': [
        "<!(node -e \"require('nan')\")",
        '/usr/local/include',
        '/usr/include',
        '/usr/local/include/quickfix',
        '/usr/include/quickfix',
        '/opt/homebrew/Cellar/boost/1.82.0_1/include'
      ],
      'direct_dependent_settings': {
        'include_dirs': ['src']
      },
      'cflags_cc': [
        "-std=c++20",
        "-fexceptions",
        "-fcxx-exceptions",
        "-D_LIBCPP_ENABLE_CXX17_REMOVED_FEATURES",
        "-DENABLE_BOOST_ATOMIC_COUNT",
        "-Wno-dynamic-exception-spec",
        "-fcxx-exceptions"
      ],
      'cflags_cc!': ['-fno-exceptions', '-fno-rtti', '-std=gnu++20'],
      'xcode_settings': {
        'OTHER_CFLAGS': [
          "-std=c++20",
          "-stdlib=libc++",
          "-fexceptions",
          "-fcxx-exceptions",
          "-D_LIBCPP_ENABLE_CXX17_REMOVED_FEATURES",
          "-Wno-dynamic-exception-spec",
          "-mmacosx-version-min=13",
        ],
      }
    },
    {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [ "<(module_name)" ],
      "copies": [
        {
          "files": [ "<(PRODUCT_DIR)/NodeQuickfix.node" ],
          "destination": "<(module_path)"
        }
      ]
    }
  ]
}
