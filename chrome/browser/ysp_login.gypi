# //ysp+

{
  'targets': [
    {
      # GN version: //chrome/browser/ysp_login
      'target_name': 'ysp_login',
      'type': 'static_library',
      'dependencies': [
        'bookmarks_common',
        'bookmarks_browser',
        '../base/base.gyp:base',
        '../base/base.gyp:base_i18n',
        '../net/net.gyp:net',
        '../ui/base/ui_base.gyp:ui_base',
        '../ui/gfx/gfx.gyp:gfx',
        '../ui/gfx/gfx.gyp:gfx_geometry',
        '../ui/gfx/gfx.gyp:gfx_vector_icons',
        '../url/url.gyp:url_lib',
        '../third_party/boringssl/boringssl.gyp:boringssl',
        'keyed_service_core',
        'pref_registry',
        'components_strings.gyp:components_strings',
        'keyed_service_core',
      ],
      'include_dirs': [
        '..',
      ],
      'sources': [
        'ysp_login/ysp_fetcher_resource.cc',
        'ysp_login/ysp_fetcher_resource.h',
        'ysp_login/ysp_iplookup_fetcher.cc',
        'ysp_login/ysp_iplookup_fetcher.h',
        'ysp_login/ysp_login_fetcher.cc',
        'ysp_login/ysp_login_fetcher.h',
        'ysp_login/ysp_replace_fetcher.cc', #//YSP+ { Resource Replace }
        'ysp_login/ysp_replace_fetcher.h', #//YSP+ { Resource Replace }
        'ysp_login/ysp_auto_config_fetcher.cc', #//YSP+ { auto get config }
        'ysp_login/ysp_auto_config_fetcher.h', #//YSP+ { auto get config }
        'ysp_login/ysp_singlesignon_fetcher.cc', #//YSP+ { SingleSignOn config }
        'ysp_login/ysp_singlesignon_fetcher.h', #//YSP+ { SingleSignOn config }
        'ysp_login/ysp_us_report_fetcher.cc',
        'ysp_login/ysp_us_report_fetcher.h',
        'ysp_login/ysp_login_manager.cc',
        'ysp_login/ysp_login_manager.h',
      ],
      'conditions': [
          ['OS == "mac"', {
              'dependencies': [
                  'ysp_sysinfo',
              ],
          }],
      ],
    },
  ],
}
