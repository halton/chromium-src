/*
 * HomeData.js  首页数据模块
 * 功能：向浏览器端、服务器端获取最新数据，之后渲染首页面
 */

var HomeData = function () {};
HomeData.prototype = function () {

    var LOGINSTATUS = 0;

    var _loadJS = function(src, callback){
        var script = document.createElement('script');
        var head = document.getElementsByTagName('head')[0];
        var loaded;
        script.src = src;
        if(typeof callback === 'function'){
            script.onload = script.onreadystatechange = function(){
                if(!loaded && (!script.readyState || /loaded|complete/.test(script.readyState))){
                    script.onload = script.onreadystatechange = null;
                    loaded = true;
                    callback();
                }
            }
        }
        head.appendChild(script);
    };

    // 从浏览器得到当前登录状态--浏览器回调方法
    var _getLoggingStatusFinish = function (loggingStatus) {
		LOGINSTATUS = loggingStatus;
        if (loggingStatus === "1") {
            $("#groups").empty();
            // 动态引入裁剪的js和css文件
			$("<link>").attr({ rel: "stylesheet",
		        type: "text/css",
		        href: "amazeui.min.css"
		    }).appendTo("head");
            _loadJS("amazeui.min.js", function(){
            	//alert("getDatajs");
                // 从浏览器获取datajs数据
                chrome.send("getParam", ["dataJS"]);
                // _getDataJSFinish();
			});
        } else {
            // 这种是不登录状态下的应用列表
            $('.notLogined-body').show();
            $('.home-body').hide();
        }
    };

    // 获取登录DATAJS完成--浏览器回调方法
    var _getDataJSFinish = function (datajs) {
        DATAJS = datajs;
        //alert("datajs:");
				//alert(datajs);
        // 从浏览器获取用户信息
        chrome.send("getParam", ["userInfo"]);
        // _getUserInfoFinish();
    };


    // 获取设备信息完成--浏览器回调方法
    var _getUserInfoFinish = function (userInfo) {
    	//alert("userInfo:");
				//alert(userInfo);
        USERINFO = userInfo;

        //根据datajs渲染页面各个功能
        _renderPageByDATAJS();
    };

    // 根据datajs渲染页面各个功能
    var _renderPageByDATAJS = function () {
        if (DATAJS !== "" && DATAJS !== undefined) {
            DATAJS = $.extend(true, {}, JSON.parse(DATAJS));
            if(DATAJS && LOGINSTATUS){
                $('.home-body').show();
                $('.notLogined-body').hide();
                //alert("_renderPageByDATAJS");
				// 创建头部公司信息
				HEADER.init();
				// 创建卡片
				// CARD.init();
				// 创建应用组
				GROUP.init();
            }
            else{
                $('.home-body').hide();
                $('.notLogined-body').show();
            }
        }
    };

    // 登录页登出时会通知此方法
	var _logout = function () {
        $('.home-body').hide();
        $('.notLogined-body').show();
		window.location.href = window.location.href;
	};

    return {
        init: function () {
            chrome.send("getParam", ["loggingStatus"]);
            // _getLoggingStatusFinish("1");
        },
		logout                   : _logout,                  // 登录页登出时会通知此方法

        getLoggingStatusFinish   : _getLoggingStatusFinish,  // 设置登录状态完成--浏览器回调方法
        getDataJSFinish          : _getDataJSFinish,         // 从浏览器得到datajs--浏览器回调方法
        getUserInfoFinish        : _getUserInfoFinish,       // 从浏览器得到用户信息--浏览器回调方法
    };
} ();
