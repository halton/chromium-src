/**
 * generate tab
 * @param {array} tab data 
 * @param {obj} set default icon,href,label 
 */
var appMarketList = [];
function generateTab(navData, accountInfo, config) {
    appMarketList =  navData;
	if (!($('#tab-wrap-market').css('display') === 'none')) {
		return;
	}
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
    return '<a class="linkitem" href="' + href + '" ><div class="linkitem-icon"><img src="'+ iconSrc +'" alt="icon" /></div><div class="linkitem-content">'+ label +'</div></a>'
  };

  var tabNavs = '';
  var tabPanels = '';

  navData.forEach(function(items, index) {
    var active = index == 0 ? 'am-active' : '';
      var nav = "<li class='" + active  + "'><a style='display: inline-block' href='javascript: void(0)'>" + items.name + "</a></li>";
    tabNavs += nav;

    var linkItems = '';
    items.applicationList.forEach(function(item) {
        if (!item.isMarketApplication || (item.isMarketApplication && item.isVisible)) {
            linkItems += itemTemplate(item);
        }
    });

      // 在每个应用组末尾添加添加应用按钮

      // 应用市场添加按钮
      linkItems +=
         '<a class="linkitem addAppButton" href="javascript:void(0)" >' +
          '<div class="linkitem-icon">' +
               '<img  src="./imgs/add_icon.svg" alt="icon" />' +
           '</div>' +
          '<div class="linkitem-content">应用管理</div>' +
         '</a>'
    
    var panel = '<div class="am-tab-panel ' + active + '"><div class="linkitems-wrap"><div class="linkitems">' + linkItems + '</div></div></div>';

    tabPanels += panel;
  })

  $('#tab-wrap').empty();

  $('#tab-wrap').append("<div class='am-tabs' id='nav-tabs'><ul class='am-tabs-nav am-nav am-nav-tabs'>" + tabNavs + "</ul><div class='am-tabs-bd'>" + tabPanels +"</div></div>");
  $('#nav-tabs').tabs({noSwipe: 1});

  $('img').error(function(){
    $(this).attr('src', "./imgs/load_failure_icon.png");
  });  
}


  // 应用市场开始————————————————————————————————————————————————————————
$(document).on("click",".addAppButton",function(){
	generateAppMarket(appMarketList);
	$("#homePage").hide();
	$("#tab-wrap-market").show();
});
// 返回应用列表
$(document).on("click","#back_app",function(){
	$("#homePage").show();
	$("#tab-wrap-market").hide();
  HOMEPROXY.getAppGroupList();
	// generateTab(appMarketList);
});

// 加载应用市场
function generateAppMarket(appMarketList) {
	var html = '';
	appMarketList.forEach(function(items, index) {
		// 创建其他应用组
		html += '<div>';
		html += '    <h3>';
		html += '       <span>'+items.name+'</span>';
		html += '    </h3>';
		html += '    <ul class="mui-table-view mui-grid-view mui-grid-9">';
		html +=           _createAPP(items.applicationList);
		html += '    </ul>';
		html += '</div>';
		$("#market_container").empty();
		$("#market_container").append(html);

	});

	function _createAPP (list) {
		var htmls     = '';
		list.forEach(function (items, index) {
			if (items.isMarketApplication) {
				var html = '';
				html += '<li>';
				html += '    <img src=' + items.logoUrl + '>';
				html += '    <h4>';
				html += '       <span>'+items.name+'</span>';
				html += '    </h4>';
				html += '    <div>';
				html += '       <input id=' + items.id + ' class="tgl tgl-light" ' + (items.isVisible? "checked" : "") + ' type="checkbox">';
				html += '       <label appId=' + items.id + ' class="tgl-btn setApp" for=' + items.id +  '></label>';
				html += '    </div>';
				html += '</li>';
				htmls += html;
			}
		});
		return htmls;
	}
	
	$('img').error(function(){
		$(this).attr('src', "./imgs/load_failure_icon.png");
	  });
}

$(document).on("click",".setApp",function(obj){
	var appId = $(this).attr("appId");
	var isVisible = !$("#" + appId).get(0).checked
	HOMEPROXY.setApplicationInfo(appId, isVisible);
});

function destroyTabs() {
  $('#nav-tabs').empty();

  $('#company_domain').hide();
  $('#company_userMessage').show();
  $('#company_QRcode').hide();
  $('#code_pack_qrcodeInvalid').hide();
  $('#code_pack_qrcodeSuccess').hide();
  $('#code_pack').hide();
}


// 应用市场结束————————————————————————————————————————————————————————




var Tabs = {};
window.Tabs = Tabs;

Tabs.generate = generateTab;
Tabs.destroy = destroyTabs;

// GET_APP_GROUP_LIST_NOTIFICATION