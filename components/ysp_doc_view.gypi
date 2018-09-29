# //ysp+

{
  'targets': [
    {
      # GN version: //components/ysp_doc_view
      'target_name': 'ysp_doc_view',
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
        '../url/url.gyp:url_lib',
        'keyed_service_core',
        'pref_registry',
        'components_strings.gyp:components_strings',
        'keyed_service_core',
      ],
      'include_dirs': [
        '..',
      ],
      'sources': [
        'ysp_doc_view/ysp_doc_view_fetcher.cc',
        'ysp_doc_view/ysp_doc_view_fetcher.h',
        'ysp_doc_view/ysp_doc_view_manager.cc',
        'ysp_doc_view/ysp_doc_view_manager.h',
      ],
    },
  ],
}
