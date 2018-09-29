/**
 * GroupApp.js  首页应用组
 * 功能：以九宫格形式展示首页APP列表
 * 描述：
 *     1. 点击 -- 点击时如果有子级APP则进入二级列表页，否则直接进入应用
 *     2. 添加 -- 添加卡片
 */

var GroupApp = function () {};
GroupApp.prototype = function () {

    // 根据datajs的数据组织组应用列表
    var _createGroup = function (dataJS) {
        var html = '';
        var hdHtml ='';
        var bdHtml ='';
        html += '<div class="am-tabs" data-am-tabs="{noSwipe: 1}">';
        html += '<ul class="am-tabs-nav am-nav am-nav-tabs">';
        var secondHdHtml = '';
        var secondBdHtml = '';
        if (dataJS.groupList && dataJS.groupList.length) {
            $("#groups").empty();
            //判断应用组是否超过10个
            if(dataJS.groupList.length< 10){
                // $('.content').height($(window).height() * 0.6);
                for (var i in dataJS.groupList) {
                    var group = dataJS.groupList[i],
                        app   = group.apps;
                        var groupName = group.name;
                    // 创建其他应用组
                    hdHtml += '<li><a href="javascript: void(0)">'+groupName+'</a></li>';
                    bdHtml += '<div class="am-tab-panel">';
                    bdHtml += '  <ul class="am-avg-md-9 apps">';
                    bdHtml +=         _createAPP(app);
                    bdHtml += '    </ul>';
                    bdHtml += '</div>';
                }
                html+= hdHtml;
                html+= '</ul>';
                html+= '<div class="am-tabs-bd">';
                html+= bdHtml;
                html+= '</div></div>'
            }
            else{
                //第一个选项卡
                for(var i =0; i< 10; i++){
                    var group = dataJS.groupList[i],
                        app   = group.apps;
                        var groupName = group.name;
                    // 创建其他应用组
                    hdHtml += '<li><a href="javascript: void(0)">'+groupName+'</a></li>';
                    bdHtml += '<div class="am-tab-panel">';
                    bdHtml += '  <ul class="am-avg-md-9 apps">';
                    bdHtml +=         _createAPP(app);
                    bdHtml += '    </ul>';
                    bdHtml += '</div>';
                }
                html += hdHtml;
                html+= '</ul>';
                html+= '<div class="am-tabs-bd">';
                html+= bdHtml;
                html+= '</div></div>';
                html += '<div class="am-tabs" data-am-tabs="{noSwipe: 1}">';
                html += '<ul class="am-tabs-nav am-nav am-nav-tabs">';

                //第二个选项卡
                for (var i = 10; i < dataJS.groupList.length; i++) {
                    var group = dataJS.groupList[i],
                        app   = group.apps;
                        var groupName = group.name;
                    // 创建其他应用组
                    secondHdHtml += '<li><a href="javascript: void(0)">'+groupName+'</a></li>';
                    secondBdHtml += '<div class="am-tab-panel">';
                    secondBdHtml += '  <ul class="am-avg-md-9 apps">';
                    secondBdHtml +=         _createAPP(app);
                    secondBdHtml += '    </ul>';
                    secondBdHtml += '</div>';
                }
                html+= secondHdHtml
                html+= '</ul>';
                html+= '<div class="am-tabs-bd">';
                html+= secondBdHtml;
                html+= '</div></div>'
            }

        } else {
            html = '<div class="no-app">您暂时没有应用组，请联系管理员</div>';
        }

        $("#groups").append(html);
        $("#groups").find('.am-tabs').tabs();
    };

    // 创建APP
    var _createAPP = function (apps) {
        var htmls     = '',
            lastClass = "";
        // 取余设置最后一个li的右边框
        for (var i in apps) {
            var html         = '',
                app          = apps[i],
                app_name     = app.appName === undefined ? "" : app.appName,
                app_logo     = app.appPCImgUrl === "" ? "images/default_icon.png" : app.appPCImgUrl,
                app_link     = app.appType === "offlineApp" ? app.appPCLink : app.appLink;

            html += '<li>';
            html += '<a href="'+app_link+'" target="_blank">'
            html += '   <img src="'+app_logo+'" />';
            html += '   <span>'+app_name+'</span>';
            html += '</a>';
            html += '</li>';

            htmls += html;
        }

        return htmls;
    };

    // 绑定事件
    var _bindEvent = function () {
        $("body").on("click", ".app img", function(){
            window.open($(this).attr("url"));
        });
    };

    // var _initHeight = function() {
    //     $('.content').height($(window).height() * 0.6);
    // }

    return {
        init: function () {
            // 创建应用组
            // _initHeight();
            _createGroup(DATAJS);
            _bindEvent();
        }
    };

} ();
