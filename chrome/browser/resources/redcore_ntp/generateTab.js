/**
 * generate tab
 * @param {array} tab data 
 * @param {obj} set default icon,href,label 
 */
function generateTab(navData, accountInfo, config) {
  var _config = $.extend({
    icon: 'imgs/defaultIcon.png',
    label: '',
    href: '#',
  }, config);

  var itemTemplate = function(data) {
      var href = data.address || _config.href;
      var iconSrc;
      if (!/\:\/\//.test(data.logoUrl)) {
          iconSrc = accountInfo.company.managerServer+ data.logoUrl
      }
      else
         iconSrc = data.logoUrl || _config.icon;
    var label = data.name || _config.label;
    return '<a class="linkitem" href="' + href + '" target="_blank"><div class="linkitem-icon"><img src="'+ iconSrc +'" alt="icon" /></div><div class="linkitem-content">'+ label +'</div></a>'
  };

  var tabNavs = '';
  var tabPanels = '';
  
  navData.forEach(function(items, index) {
    var active = index == 0 ? 'am-active' : '';

    var nav = "<li class='" + active  + "'><a href='javascript: void(0)'>" + items.name + "</a></li>";

    tabNavs += nav;

    var linkItems = '';
    items.applicationList.forEach(function(item) {
       linkItems += itemTemplate(item);
    });
    
    var panel = '<div class="am-tab-panel ' + active + '"><div class="linkitems-wrap"><div class="linkitems">' + linkItems + '</div></div></div>';

    tabPanels += panel;
  })

  $('#tab-wrap').empty();

  $('#tab-wrap').append("<div class='am-tabs' id='nav-tabs'><ul class='am-tabs-nav am-nav am-nav-tabs'>" + tabNavs + "</ul><div class='am-tabs-bd'>" + tabPanels +"</div></div>");
  $('#nav-tabs').tabs({noSwipe: 1});
}

function destroyTabs() {
  $('#nav-tabs').empty();

  $('#company_domain').hide();
  $('#company_userMessage').show();
  $('#company_QRcode').hide();
  $('#code_pack_qrcodeInvalid').hide();
  $('#code_pack_qrcodeSuccess').hide();
  $('#code_pack').hide();
}

var Tabs = {};
window.Tabs = Tabs;

Tabs.generate = generateTab;
Tabs.destroy = destroyTabs;
