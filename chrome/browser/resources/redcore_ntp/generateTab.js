<<<<<<< HEAD
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
||||||| parent of ffef05f456bc... Add untracked files after rebase
=======
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
    return '<a class="linkitem" href="' + href + '" ><div class="linkitem-icon"><img src="'+ iconSrc +'" alt="icon" /></div><div class="linkitem-content">'+ label +'</div></a>'
  };

  var tabNavs = '';
  var tabPanels = '';

  // 设置应用组长度样式
    if (navData.length > 6) {
        var appGroupWidth = "width:" + (parseInt((document.body.clientWidth  - 200) / 2 / navData.length) * 100 /parseInt(document.body.clientWidth)) + '%';
    } else {
        var appGroupWidth = "width:" + (document.body.clientWidth - 200) / 2 / navData.length + 'px';
    }

  navData.forEach(function(items, index) {
    var active = index == 0 ? 'am-active' : '';

      if (navData.length > 6) {
          var nav = "<li style=" + appGroupWidth +" class='" + active  + "'><a style='display: block' href='javascript: void(0)'>" + items.name + "</a></li>";
      } else {
          var nav = "<li style=" + appGroupWidth +" class='" + active  + "'><a style='display: inline-block' href='javascript: void(0)'>" + items.name + "</a></li>";
      }

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


// // 首页数据
// var applists = [
//     {
//         "applicationList": [
//             {
//                 "address": "http://www.yunshipei.com/index.html",
//                 "createDate": "2018-03-23T04:55:36.000Z",
//                 "description": "",
//                 "id": "d28d1640-69c5-4aa0-b2f1-0373a6d4c901",
//                 "identifier": null,
//                 "lastUpdate": "2018-03-23T04:55:36.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/m47eq9k468j.png",
//                 "name": "红芯云适配官网",
//                 "packageUrl": null,
//                 "type": 1
//             },
//             {
//                 "address": "https://www.redcore.cn/",
//                 "createDate": "2018-03-23T04:55:17.000Z",
//                 "description": "",
//                 "id": "f1b3d8fe-5d9b-48e5-bff2-3f31fb082c77",
//                 "identifier": null,
//                 "lastUpdate": "2018-04-28T14:40:54.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/rfg49moxga8.png",
//                 "name": "红芯企业浏览器官网",
//                 "packageUrl": null,
//                 "type": 1
//             },
//             {
//                 "address": "http://ask.yunshipei.info/",
//                 "createDate": "2018-03-23T02:43:15.000Z",
//                 "description": "",
//                 "id": "a71bfc63-4159-4720-80b8-8f97d4fc6ee5",
//                 "identifier": null,
//                 "lastUpdate": "2018-03-23T02:43:15.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/6zf82y5rpv.png",
//                 "name": "开发者社区",
//                 "packageUrl": null,
//                 "type": 1
//             },
//             {
//                 "address": "https://git.yunshipei.com/users/sign_in",
//                 "createDate": "2017-12-23T06:24:48.000Z",
//                 "description": "",
//                 "id": "cfe620bd-b1c5-42e4-a296-6ed6a0571929",
//                 "identifier": null,
//                 "lastUpdate": "2017-12-26T04:50:07.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/8xcdoxyu1wg.png",
//                 "name": "Git",
//                 "packageUrl": null,
//                 "type": 1
//             },
//             {
//                 "address": "https://crm.xiaoshouyi.com/global/login.action",
//                 "createDate": "2017-12-23T05:51:29.000Z",
//                 "description": "",
//                 "id": "4a5b2966-beb0-40a1-b532-a5d649dde3a2",
//                 "identifier": null,
//                 "lastUpdate": "2017-12-26T04:47:09.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/srxtz0ec6mj.png",
//                 "name": "销售易",
//                 "packageUrl": null,
//                 "type": 1
//             },
//             {
//                 "address": "http://mail.redcore.cn/",
//                 "createDate": "2017-12-23T05:49:48.000Z",
//                 "description": "",
//                 "id": "7125c995-45fe-4cab-8c34-367c0250a4be",
//                 "identifier": null,
//                 "lastUpdate": "2018-03-01T02:44:57.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/bbnp2jco4nv.png",
//                 "name": "工作邮箱",
//                 "packageUrl": null,
//                 "type": 1
//             },
//             {
//                 "address": "http://wiki.yunshipei.info/login.action?os_destination=%2Fdashboard.action",
//                 "createDate": "2017-12-23T05:48:43.000Z",
//                 "description": "",
//                 "id": "226a9537-104c-4fa6-b8a5-202485b8bcd0",
//                 "identifier": null,
//                 "lastUpdate": "2018-01-25T11:27:36.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/iblywmqasi.png",
//                 "name": "Wiki知识库",
//                 "packageUrl": null,
//                 "type": 1
//             },
//             {
//                 "address": "http://oa.yunshipei.com/OAapp/WebObjects/OAapp.woa",
//                 "createDate": "2017-12-23T05:45:52.000Z",
//                 "description": "",
//                 "id": "0496b10b-3765-4914-997f-32ddac746a2a",
//                 "identifier": null,
//                 "lastUpdate": "2017-12-26T04:45:44.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/0uex0nla171.png",
//                 "name": "OA",
//                 "packageUrl": null,
//                 "type": 1
//             },
//             {
//                 "address": "http://jira.yunshipei.info/login.jsp",
//                 "createDate": "2017-12-23T05:44:30.000Z",
//                 "description": "",
//                 "id": "0d0e1f46-002b-43d6-9a2e-a6e140f2815a",
//                 "identifier": null,
//                 "lastUpdate": "2017-12-26T04:46:56.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/hhl7cvmyfg8.png",
//                 "name": "JIRA",
//                 "packageUrl": null,
//                 "type": 1
//             }
//         ],
//         "id": "0d1f9d02-e788-11e7-b9d0-94de80d52441",
//         "index": 1,
//         "name": "默认应用组",
//         "type": 1
//     },
//     {
//         "applicationList": [
//             {
//                 "address": "http://app2.yunshipei.com/download/彩印机黑白机驱动安装指南.docx",
//                 "createDate": "2018-04-27T09:50:37.000Z",
//                 "description": "",
//                 "id": "7f475bee-15a2-4778-9c95-0f4c47075532",
//                 "identifier": null,
//                 "lastUpdate": "2018-05-03T07:13:45.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/89d0l8mto14.png",
//                 "name": "打印机驱动安装指南",
//                 "packageUrl": "",
//                 "type": 1
//             },
//             {
//                 "address": "http://wiki.yunshipei.info/pages/viewpage.action?pageId=4229724",
//                 "createDate": "2018-02-10T07:54:26.000Z",
//                 "description": "",
//                 "id": "a8e0e937-618a-4f2e-a891-33bf1cf21e63",
//                 "identifier": null,
//                 "lastUpdate": "2018-02-10T07:54:26.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/l1tmw1f67v.png",
//                 "name": "红芯设计素材",
//                 "packageUrl": null,
//                 "type": 1
//             },
//             {
//                 "address": "http://wiki.yunshipei.info/pages/viewpage.action?pageId=4229886",
//                 "createDate": "2018-02-01T06:36:41.000Z",
//                 "description": "",
//                 "id": "6f010f55-f71c-4990-b08d-d63c397ada3b",
//                 "identifier": null,
//                 "lastUpdate": "2018-02-27T02:48:20.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/fv60nhetzk8.png",
//                 "name": "POC模板（云安全）",
//                 "packageUrl": null,
//                 "type": 1
//             },
//             {
//                 "address": "http://wiki.yunshipei.info/pages/viewpage.action?pageId=4229410",
//                 "createDate": "2018-01-26T07:35:54.000Z",
//                 "description": "",
//                 "id": "a049e526-490d-4d02-a6a3-7e29d2ed6acd",
//                 "identifier": null,
//                 "lastUpdate": "2018-02-01T06:37:13.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/czzga4bs7g9.png",
//                 "name": "POC模板（云适配）",
//                 "packageUrl": null,
//                 "type": 1
//             },
//             {
//                 "address": "http://wiki.yunshipei.info/pages/viewpage.action?pageId=4229726",
//                 "createDate": "2018-01-25T11:05:47.000Z",
//                 "description": "",
//                 "id": "7df25edd-1951-4c39-969c-0dd35fe6e495",
//                 "identifier": null,
//                 "lastUpdate": "2018-02-10T07:53:35.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/0g3hqyqxuhh.jpg",
//                 "name": "PPT模板",
//                 "packageUrl": null,
//                 "type": 1
//             },
//             {
//                 "address": "http://wiki.yunshipei.info/pages/viewpage.action?pageId=1015947",
//                 "createDate": "2018-01-25T10:58:56.000Z",
//                 "description": "",
//                 "id": "acef340f-e35e-4f22-bdc3-c14d784c8371",
//                 "identifier": null,
//                 "lastUpdate": "2018-03-05T07:26:00.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/fykdvwvaybw.png",
//                 "name": "红芯产品介绍",
//                 "packageUrl": null,
//                 "type": 1
//             },
//             {
//                 "address": "http://wiki.yunshipei.info/pages/viewpage.action?pageId=2458577",
//                 "createDate": "2018-01-25T10:56:15.000Z",
//                 "description": "",
//                 "id": "33773395-379e-4123-afbd-eeedf28deea9",
//                 "identifier": null,
//                 "lastUpdate": "2018-01-25T10:56:15.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/yl8hoszgdr.png",
//                 "name": "人事制度",
//                 "packageUrl": null,
//                 "type": 1
//             },
//             {
//                 "address": "http://wiki.yunshipei.info/pages/viewpage.action?pageId=2459415",
//                 "createDate": "2018-01-25T10:43:55.000Z",
//                 "description": "",
//                 "id": "59cb913f-f0e2-488b-8b16-a3bdefecb0f2",
//                 "identifier": null,
//                 "lastUpdate": "2018-01-25T13:39:11.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/mqhb0irp93r.png",
//                 "name": "财务管理制度",
//                 "packageUrl": null,
//                 "type": 1
//             }
//         ],
//         "id": "4bb763f5-73a0-413d-a74a-8dc0d73b0127",
//         "index": 1,
//         "name": "常用文档",
//         "type": 2
//     },
//     {
//         "applicationList": [
//             {
//                 "address": "https://account.yunshipei.com/iisadmpwd/",
//                 "createDate": "2017-12-23T06:29:18.000Z",
//                 "description": "OA及Enterplorer浏览器修改密",
//                 "id": "9e4097bd-ab2a-4200-909b-88e194ced7cf",
//                 "identifier": null,
//                 "lastUpdate": "2018-01-25T12:09:05.000Z",
//                 "level": 1,
//                 "logoUrl": "http://ysp.redcore.cn/files/appiconlogos/5a3b790bf9c071b3be109304/liz1zz7awz.png",
//                 "name": "OA密码修改",
//                 "packageUrl": null,
//                 "type": 1
//             }
//         ],
//         "id": "2276fe35-b794-4ded-a2fd-67b083bb623e",
//         "index": 1,
//         "name": "管理员应用",
//         "type": 2
//     },
// ];
// generateTab (applists);


var Tabs = {};
window.Tabs = Tabs;

Tabs.generate = generateTab;
Tabs.destroy = destroyTabs;
>>>>>>> ffef05f456bc... Add untracked files after rebase
