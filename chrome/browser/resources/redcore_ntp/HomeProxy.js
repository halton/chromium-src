/*
 * HomeProxy.js  首页数据模块
 * 功能：向浏览器端、服务器端获取最新数据，之后渲染首页面
 */

var HOMEBROWSER = null;
var HomeProxy = function () { };

HomeProxy.prototype = function () {

    var LOGINSTATUS = null;  // 保留当前用户登录状态
    var BROWSERNOTIFICATION = null; // 保留与浏览器通信模块的instance。

    //自定义通告构造函数
    function BrowserNotification() {
        //通告处理程序数组集合
        this.handlers = {};
    }
    //自定义通告的原型对象
    BrowserNotification.prototype = {
        //设置原型构造函数链
        constructor: BrowserNotification,
        //注册给定类型的通告处理程序，
        //type -> 自定义通告类型，target->定义通告发往的对象， handler -> 自定义通告回调函数
        registNotification: function (type, target, handler) {
            //判断通告处理数组是否有该类型通告
            if (typeof this.handlers[type] == 'undefined') {
                this.handlers[type] = [];
            }
            //将处理通告push到通告处理数组里面
            this.handlers[type].push([target, handler]);
        },

        //触发一个通告
        fireNotification: function (notification) {
            //判断是否存在该通告类型
            if (this.handlers[notification.type] instanceof Array) {
                var handlers = this.handlers[notification.type];
                //在同一个通告类型下的可能存在多种处理通告，找出本次需要处理的通告
                for (var i = 0; i < handlers.length; i++) {
                    //执行触发
                    var ret = notification.return
                    var handler = handlers[i][1];
                    if (ret != undefined)
                        setTimeout(function () { handler(ret); }, 0);
                    else
                        setTimeout(function () { handler(); }, 0);
                }
            }
        },

        //注销通告
        //type -> 自定义通告类型，target->定义通告发往的对象， handler -> 自定义通告回调函数
        removeNotification: function (type, target, handler) {
            if (this.handlers[type] instanceof Array) {
                var handlers = this.handlers[type];
                //在同一个通告类型下的可能存在多种处理通告
                for (var i = 0; i < handlers.length; i++) {
                    //找出本次需要处理的通告下标
                    if (handlers[i][0] == target && handler != undefined && handlers[i][1] == handler) 
                        handlers.splice(i, 1);
                    else if (handlers[i][0] == target && handler == undefined)
                        handlers.splice(i, 1);
                }
            }
        }
    };

    // 从浏览器得到当前登录状态--浏览器回调方法
    var _getLoggingStatusCallback = function (loggingStatus, msg) {
        if (LOGINSTATUS != loggingStatus || msg != undefined) {
            LOGINSTATUS = loggingStatus;
            BROWSERNOTIFICATION.fireNotification({ type: "LOGGING_STATUS_NOTIFICATION", return : {status: loggingStatus, message: msg }});
        }
    };

      // 当前用户退出后回调此方法
    var _logOutCallback = function () {
        LOGINSTATUS = null;
        ACCOUNTINFO = {};
        // BROWSERNOTIFICATION.fireNotification({ type: "LOGOUTED_NOTIFICATION", return: address });
        BROWSERNOTIFICATION.fireNotification({ type: "LOGOUTED_NOTIFICATION"});
    };

    // 添加指定调用的回调。
    var _registNotification = function (type, target, handler) {
	    try {
	        if (type == "GET_DEVICEID_NOTIFICATION" ||
	            type == "GET_FIRST_LOGIN_NOTIFICATION" ||
	            type == "LOGGING_STATUS_NOTIFICATION" ||
                type == "GET_ACCOUNT_INFO_NOTIFICATION" ||
                type == "GET_APP_GROUP_LIST_NOTIFICATION" ||
                type == "GET_AUTO_LOGIN_NOTIFICATION" ||
                type == "SET_AUTO_LOGIN_NOTIFICATION" ||
                type == "GET_MANAGER_ADDRESS_NOTIFICATION" ||
                type == "SET_MANAGER_ADDRESS_NOTIFICATION" ||
                type == "LOGOUTED_NOTIFICATION" ||
                type == "GET_LAST_LOGIN_INFO_NOTIFICATION") {
	            BROWSERNOTIFICATION.registNotification(type, target, handler);
	        }
	        else
                throw type + " is not supported!"
	    }
	    catch (err) {
	        alert(err);
	    }
	};

    // 删除指定调用的回调。
    var _unregistNotification = function (type, target, handler) {
	    try {
	        if (type == "GET_DEVICEID_NOTIFICATION" ||
	            type == "GET_FIRST_LOGIN_NOTIFICATION" ||
	            type == "LOGGING_STATUS_NOTIFICATION" ||
                type == "GET_ACCOUNT_INFO_NOTIFICATION" ||
                type == "GET_APP_GROUP_LIST_NOTIFICATION" ||
                type == "GET_AUTO_LOGIN_NOTIFICATION" ||
                type == "SET_AUTO_LOGIN_NOTIFICATION" ||
                type == "GET_MANAGER_ADDRESS_NOTIFICATION" ||
                type == "SET_MANAGER_ADDRESS_NOTIFICATION" ||
                type == "LOGOUTED_NOTIFICATION" ||
                type == "GET_LAST_LOGIN_INFO_NOTIFICATION") {
	            BROWSERNOTIFICATION.removeNotification(type, target, handler);
	        }
	        else
	            throw type + " is not supported!"
	    }
	    catch (err) {
	        alert(err);
	    }
	};

	var proxyCallBack = {
        // 从浏览器得到deviceId--浏览器回调方法
        getDeviveIdCallback: function (deviceId) { BROWSERNOTIFICATION.fireNotification({ type: "GET_DEVICEID_NOTIFICATION", return: deviceId }); },
        // 从浏览器得到是否是第一次打开--浏览器回调方法
        getFirstLoginCallback: function (firstLogin) { BROWSERNOTIFICATION.fireNotification({ type: "GET_FIRST_LOGIN_NOTIFICATION", return: firstLogin }); },
	    // 设置登录状态完成--浏览器回调方法
		getLoggingStatusCallback   : _getLoggingStatusCallback,
	    // 从浏览器得到datajs--浏览器回调方法
		getAppGroupListCallback    : function (list) { BROWSERNOTIFICATION.fireNotification({ type: "GET_APP_GROUP_LIST_NOTIFICATION", return: list }); },
	    // 从浏览器得到用户信息--浏览器回调方法
		getAccountInfoCallback: function (info) { BROWSERNOTIFICATION.fireNotification({ type: "GET_ACCOUNT_INFO_NOTIFICATION", return: info }); },
	    // 设置自动登录状态完成时浏览器回调此方法
        setAutoLoginCallback       : function () { BROWSERNOTIFICATION.fireNotification({ type: "SET_AUTO_LOGIN_NOTIFICATION" }); }, 
	    // 从浏览器得到自动登录状态--浏览器回调方法
        getAutoLoginCallback       : function (autoLogin) { BROWSERNOTIFICATION.fireNotification({ type: "GET_AUTO_LOGIN_NOTIFICATION", return: autoLogin }); },
	    // 设置EnterManager地址--浏览器回调方法
        setManagerAddressCallback  : function (address) { BROWSERNOTIFICATION.fireNotification({ type: "SET_MANAGER_ADDRESS_NOTIFICATION" }); },
	    // 从浏览器得到EnterManager地址--浏览器回调方法
        getManagerAddressCallback  : function (address) { BROWSERNOTIFICATION.fireNotification({ type: "GET_MANAGER_ADDRESS_NOTIFICATION", return: address }); },
	    // 从浏览器得到上次登录时的账号信息--浏览器回调方法
        getLastLoginInfoCallback: function (info) { BROWSERNOTIFICATION.fireNotification({ type: "GET_LAST_LOGIN_INFO_NOTIFICATION", return: info }); },
	    // 当前用户退出登录
        logOutCallback             : _logOutCallback
	};

	return {
        // 首页调用此方法初始化HomeProxy及其相关模块
	    init: function () {
	        BROWSERNOTIFICATION = new BrowserNotification();
	        HOMEBROWSER = new HomeBrowser();
	        HOMEBROWSER.init(proxyCallBack);
	    },

	    // 用户登录时调用此方法。登录状态在LOGGING_STATUS_NOTIFICATION通知中
	    login                    : function (domain, email, password) { HOMEBROWSER.command("login", [domain, email, password]); },
	    // 用户退出登录时调用此方法。退出完成后HomeProxy中的Cache被清空。
        logout                   : function () { HOMEBROWSER.command("logout"); },

	    // 上层调用此方法设置自动登录状态，调用完成后触发SET_AUTO_LOGIN_NOTIFICATION通知。
		setAutoLogin             : function (autoLogin) { HOMEBROWSER.command("setParam", ["autoLogin", autoLogin]); }, 
	    // 上层调用此方法获得自动登录状态，调用完成后触发GET_AUTO_LOGIN_NOTIFICATION通知。
        getAutoLogin             : function () { HOMEBROWSER.command("getParam", ["autoLogin"]) }, 
	    // 上层调用此方法设置服务器地址,调用完成后触发SET_MANAGER_ADDRESS_NOTIFICATION通知。
        setManagerAddress        : function (address) { HOMEBROWSER.command("setParam", ["managerAddress", address]); },
	    // 上层调用此方法获得服务器地址, 调用完成后触发GET_MANAGER_ADDRESS_NOTIFICATION通知。
        getManagerAddress        : function () { HOMEBROWSER.command("getParam", ["managerAddress"]); },

        // 上层调用此方法获得当前用户deviceId,  调用完成后触发LOGGING_STATUS_NOTIFICATION通知。
        getDeviceId: function () { HOMEBROWSER.command("getParam", ["deviceId"]); },

        // 上层调用此方法获得当前用户是否第一次打开,  调用完成后触发LOGGING_STATUS_NOTIFICATION通知。
        getFirstLogin: function () { HOMEBROWSER.command("getParam", ["firstLogin"]); },

	    // 上层调用此方法获得当前用户登录状态,  调用完成后触发LOGGING_STATUS_NOTIFICATION通知。
        getLoggingStatus: function () { LOGINSTATUS = null; HOMEBROWSER.command("getParam", ["loggingStatus"]); },
	    // 上层调用此方法获得上次登录时的信息,  调用完成后触发GET_LAST_LOGIN_INFO_NOTIFICATION通知。
        getLastLoginInfo: function () { HOMEBROWSER.command("getParam", ["lastLoginInfo"]); },

	    // 上层调用此方法获得用户信息， 调用完成后触发GET_APP_GROUP_LIST_NOTIFICATION通知。
        getAppGroupList: function () { HOMEBROWSER.command("getParam", ["appGroupList"]); },

	    // 上层调用此方法获得用户信息， 调用完成后触发GET_ACCOUNT_INFO_NOTIFICATION通知。
        getAccountInfo: function () { HOMEBROWSER.command("getParam", ["accountInfo"]); },
	    // 上层调用此方法注册指定的通知
        // 目前支持的通知事件有：
	    //     LOGGING_STATUS_NOTIFICATION
	    //     GET_ACCOUNT_INFO_NOTIFICATION
	    //     GET_APP_GROUP_LIST_NOTIFICATION
	    //     GET_AUTO_LOGIN_NOTIFICATION
	    //     SET_AUTO_LOGIN_NOTIFICATION
	    //     GET_MANAGER_ADDRESS_NOTIFICATION
	    //     SET_MANAGER_ADDRESS_NOTIFICATION
	    //     LOGOUTED_NOTIFICATION
        registNotification       : _registNotification,
	    // 上层调用此方法删除已经注册的指定通知事件。       
        unregistNotification     : _unregistNotification,
	};
} ();

