<<<<<<< HEAD
var HomeBrowser = function () {};
window.HomeBrowser = HomeBrowser

HomeBrowser.prototype = function () {
    var PROXYCALLBACK = null;
    // 浏览器的设置参数完成的回调方法
    var _setParamCallback = function (key) {
        if (PROXYCALLBACK == null)
            return;
        // 根据key去分别调用不同模块的业务方法
        switch (key) {
            case "loggingStatus":
                PROXYCALLBACK.setLoggingStatusCallback();
                break;
            case "autoLogin":
                PROXYCALLBACK.setAutoLoginCallback();
                break;
            case "managerAddress":
                PROXYCALLBACK.setManagerAddressCallback();
                break;
            case "domain":
                PROXYCALLBACK.setDomainCallback();
                break;
            case "companyId":
                PROXYCALLBACK.setCompanyIdCallback();
                break;
            case "userInfo":
                PROXYCALLBACK.setUserInfoCallback();
                break;
            case "password":
                PROXYCALLBACK.setPasswordCallback();
                break;
        }
    };

    // 浏览器的获取参数完成的回调方法
    var _getParamCallback = function (key, value) {
        console.log('_getParamCallback', key, value)
        if (PROXYCALLBACK == null)
            return;
        // 根据key去分别调用不同模块的业务方法
        switch (key) {
            case "loggingStatus":
                PROXYCALLBACK.getLoggingStatusCallback(new Number(value));
                break;
            case "autoLogin":
                PROXYCALLBACK.getAutoLoginCallback(value == "1" ? true : false);
                break;
            case "managerAddress":
                PROXYCALLBACK.getManagerAddressCallback(value);
                break;
            case "domain":
                PROXYCALLBACK.getDomainCallback(value);
                break;
            case "companyId":
                PROXYCALLBACK.getCompanyIdCallback(value);
                break;
            case "configData":
                if (value)
                {
                    var configData = $.extend(true, {}, JSON.parse(value));
                    var ret = {};
                    ret.companyTitle =  configData.companyTitle;
                    ret.iconUrl = configData.iconUrl;
                    ret.groupList = configData.groupList;
                    ret.backgroundUrl = ""; // TODO: should get it from manager.
                    PROXYCALLBACK.getConfigDataCallback(ret);
                }
                break;
            case "userInfo":
                if (value)
                {
                    var userInfo = JSON.parse(value);
                    var ret = {};
                    ret.name = userInfo.name;
                    ret.tel = userInfo.tel;
                    ret.email = userInfo.email;
                    PROXYCALLBACK.getUserInfoCallback(ret);
                }
                break;
            case "password":
                PROXYCALLBACK.getPasswordCallback(value);
                break;
            case "lastLoginInfo":
                PROXYCALLBACK.getLastLoginInfoCallback(JSON.parse(value));
                break;
        }
    };

    // 当登录完成后浏览器回调此接口
    var _configDataChanged = function (msg) {
		if (msg === "login") {
		    chrome.send("getParam", ["configData"]);
		    chrome.send("getParam", ["userInfo"]);
		} else if (msg === "logout") {
		    PROXYCALLBACK.logOutCallback();
        } else if (msg != undefined) {
            PROXYCALLBACK.getLoggingStatusCallback(0, msg);
        }
    };

    return {
        // 上层调用此方法初始化与浏览器通信模块
        init : function (callback) {
            PROXYCALLBACK = callback;
            chrome.send("setParam", ["initialized"]);
        },
        // 上层调用此方法来调用相应WebUI Native的接口 
        command: function (cmd, args) { chrome.send(cmd, args) },

        // 浏览器设置参数调用完成后，回调此方法
        setParamCallback          : _setParamCallback,
        // 浏览器获取参数调用完成后，回调此方法
        getParamCallback          : _getParamCallback,
        // 获得设备信息的回调方法
        getDeviceInfoCallback     : function (deviceInfo) { PROXYCALLBACK.getDeviceInfoCallback(deviceInfo); },
        // 当浏览器获取当前城市后回调此接口
        getCityCallback           : function (cityInfo) { PROXYCALLBACK.getCityInfoCallback(cityInfo); },
        // 当前用户配置信息发生改变时调用此接口
        configDataChanged         : _configDataChanged,
        // 当浏览器登录状态改变后调用此接口
        loginStatusChanged        : function(){ chrome.send("getParam", ["loggingStatus"]); }
    };
}();
||||||| parent of ffef05f456bc... Add untracked files after rebase
=======
var HomeBrowser = function () {};
window.HomeBrowser = HomeBrowser

HomeBrowser.prototype = function () {
    var PROXYCALLBACK = null;
    // 浏览器的设置参数完成的回调方法
    var _setParamCallback = function (key) {
        if (PROXYCALLBACK == null)
            return;
        // 根据key去分别调用不同模块的业务方法
        switch (key) {
            case "loggingStatus":
                PROXYCALLBACK.setLoggingStatusCallback();
                break;
            case "autoLogin":
                PROXYCALLBACK.setAutoLoginCallback();
                break;
            case "managerAddress":
                PROXYCALLBACK.setManagerAddressCallback();
                break;
            case "domain":
                PROXYCALLBACK.setDomainCallback();
                break;
            case "companyId":
                PROXYCALLBACK.setCompanyIdCallback();
                break;
            case "userInfo":
                PROXYCALLBACK.setUserInfoCallback();
                break;
            case "password":
                PROXYCALLBACK.setPasswordCallback();
                break;
        }
    };

    // 浏览器的获取参数完成的回调方法
    var _getParamCallback = function (key, value) {
        console.log('_getParamCallback', key, value)
        if (PROXYCALLBACK == null)
            return;
        // 根据key去分别调用不同模块的业务方法
        switch (key) {
            case "loggingStatus":
                PROXYCALLBACK.getLoggingStatusCallback(new Number(value));
                break;
            case "autoLogin":
                PROXYCALLBACK.getAutoLoginCallback(value == "1" ? true : false);
                break;
            case "managerAddress":
                PROXYCALLBACK.getManagerAddressCallback(value);
                break;
            case "domain":
                PROXYCALLBACK.getDomainCallback(value);
                break;
            case "companyId":
                PROXYCALLBACK.getCompanyIdCallback(value);
                break;
            case "configData":
                if (value)
                {
                    var configData = $.extend(true, {}, JSON.parse(value));
                    var ret = {};
                    ret.companyTitle =  configData.companyTitle;
                    ret.iconUrl = configData.iconUrl;
                    ret.groupList = configData.groupList;
                    ret.backgroundUrl = ""; // TODO: should get it from manager.
                    PROXYCALLBACK.getConfigDataCallback(ret);
                }
                break;
            case "userInfo":
                if (value)
                {
                    var userInfo = JSON.parse(value);
                    var ret = {};
                    ret.name = userInfo.name;
                    ret.tel = userInfo.tel;
                    ret.email = userInfo.email;
                    PROXYCALLBACK.getUserInfoCallback(ret);
                }
                break;
            case "password":
                PROXYCALLBACK.getPasswordCallback(value);
                break;
            case "lastLoginInfo":
                PROXYCALLBACK.getLastLoginInfoCallback(JSON.parse(value));
                break;
        }
    };

    // 当登录完成后浏览器回调此接口
    var _configDataChanged = function (msg) {
		if (msg === "login") {
		    chrome.send("getParam", ["configData"]);
		    chrome.send("getParam", ["userInfo"]);
		} else if (msg === "logout") {
		    PROXYCALLBACK.logOutCallback();
        } else if (msg != undefined) {
            PROXYCALLBACK.getLoggingStatusCallback(0, msg);
        }
    };

    return {
        // 上层调用此方法初始化与浏览器通信模块
        init : function (callback) {
            PROXYCALLBACK = callback;
            chrome.send("setParam", ["initialized"]);
        },
        // 上层调用此方法来调用相应WebUI Native的接口 
        command: function (cmd, args) { chrome.send(cmd, args) },

        // 浏览器设置参数调用完成后，回调此方法
        setParamCallback          : _setParamCallback,
        // 浏览器获取参数调用完成后，回调此方法
        getParamCallback          : _getParamCallback,
        // 获得设备信息的回调方法
        getDeviceInfoCallback     : function (deviceInfo) { PROXYCALLBACK.getDeviceInfoCallback(deviceInfo); },
        // 当浏览器获取当前城市后回调此接口
        getCityCallback           : function (cityInfo) { PROXYCALLBACK.getCityInfoCallback(cityInfo); },
        // 当前用户配置信息发生改变时调用此接口
        configDataChanged         : _configDataChanged,
        // 当浏览器登录状态改变后调用此接口
        loginStatusChanged        : function(){ chrome.send("getParam", ["loggingStatus"]); }
    };
}();
>>>>>>> ffef05f456bc... Add untracked files after rebase
