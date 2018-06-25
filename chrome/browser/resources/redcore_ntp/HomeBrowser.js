<<<<<<< HEAD
/**
 * HomeBrowser.js  与浏览器交互的功能js
 */

var HomeBrowser = function () {};
HomeBrowser.prototype = function () {
    var PROXYCALLBACK = null;
    // 浏览器的设置参数完成的回调方法

    function trimSchema(str)
    {
        var http = "http://";
        var https = "https://";
        if (str.substr(0, http.length) == http)
            str = str.slice(http.length);
        else if (str.substr(0, https.length) == https)
            str = str.slice(https.length);
        return str;
    }

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
            case "accountInfo":
                PROXYCALLBACK.setAccountInfoCallback();
                break;
            case "password":
                PROXYCALLBACK.setPasswordCallback();
                break;
        }
    };

    // 浏览器的获取参数完成的回调方法
    var _getParamCallback = function (key, value) {
        if (PROXYCALLBACK == null)
            return;
        // 根据key去分别调用不同模块的业务方法
        switch (key) {
            case "deviceId":
                PROXYCALLBACK.getDeviveIdCallback(value || '');
                break;
            case "firstLogin":
                PROXYCALLBACK.getFirstLoginCallback(value == "1" ? true : false);
                break;
            case "loggingStatus":
                PROXYCALLBACK.getLoggingStatusCallback(new Number(value));
                break;
            case "autoLogin":
                PROXYCALLBACK.getAutoLoginCallback(value == "1" ? true : false);
                break;
            case "managerAddress":
                if (value)
                    value = trimSchema(value);
                PROXYCALLBACK.getManagerAddressCallback(value);
                break;
            case "domain":
                PROXYCALLBACK.getDomainCallback(value);
                break;
            case "companyId":
                PROXYCALLBACK.getCompanyIdCallback(value);
                break;
            case "appGroupList":
                if (value)
                {
                    var ret = JSON.parse(value);
                    if (ret)
                        PROXYCALLBACK.getAppGroupListCallback(ret);
                }
                break;
            case "accountInfo":
                if (value)
                {
                    var ret = JSON.parse(value);
                    if (ret)
                        PROXYCALLBACK.getAccountInfoCallback(ret);
                }
                break;
            case "password":
                PROXYCALLBACK.getPasswordCallback(value);
                break;
            case "lastLoginInfo":
                value = JSON.parse(value);
                if (value && value.domain)
                    value.domain = trimSchema(value.domain);
                PROXYCALLBACK.getLastLoginInfoCallback(value);
                break;
        }
    };

    // 当登录完成后浏览器回调此接口
    var _configDataChanged = function (data, msg) {
		if (msg === "login") {
		    chrome.send("getParam", ["accountInfo"]);
		    chrome.send("getParam", ["appGroupList"]);
		    PROXYCALLBACK.getLoggingStatusCallback(new Number(data));
        } else if (msg === "logout") {
		    PROXYCALLBACK.logOutCallback();
        } else if (msg === "application") {
            if (data) {
                var ret = JSON.parse(data);
                if (ret)
                    PROXYCALLBACK.getAppGroupListCallback(ret);
            }
        } else if (msg === "accountInfo") {
            if (data) {
                var ret = JSON.parse(data);
                if (ret)
                    PROXYCALLBACK.getAccountInfoCallback(ret);
            }
        } else if (msg === "strategy") {
            if (data) {
                var ret = JSON.parse(data);
                if (ret)
                    PROXYCALLBACK.getAppGroupListCallback(ret);
            }
        } else {
            PROXYCALLBACK.getLoggingStatusCallback(new Number(data), msg);
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
} ();
||||||| parent of ffef05f456bc... Add untracked files after rebase
=======
/**
 * HomeBrowser.js  与浏览器交互的功能js
 */

var HomeBrowser = function () {};
HomeBrowser.prototype = function () {
    var PROXYCALLBACK = null;
    // 浏览器的设置参数完成的回调方法

    function trimSchema(str)
    {
        var http = "http://";
        var https = "https://";
        if (str.substr(0, http.length) == http)
            str = str.slice(http.length);
        else if (str.substr(0, https.length) == https)
            str = str.slice(https.length);
        return str;
    }

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
            case "accountInfo":
                PROXYCALLBACK.setAccountInfoCallback();
                break;
            case "password":
                PROXYCALLBACK.setPasswordCallback();
                break;
        }
    };

    // 浏览器的获取参数完成的回调方法
    var _getParamCallback = function (key, value) {
        if (PROXYCALLBACK == null)
            return;
        // 根据key去分别调用不同模块的业务方法
        switch (key) {
            case "deviceId":
                PROXYCALLBACK.getDeviveIdCallback(value || '');
                break;
            case "firstLogin":
                PROXYCALLBACK.getFirstLoginCallback(value == "1" ? true : false);
                break;
            case "loggingStatus":
                PROXYCALLBACK.getLoggingStatusCallback(new Number(value));
                break;
            case "autoLogin":
                PROXYCALLBACK.getAutoLoginCallback(value == "1" ? true : false);
                break;
            case "managerAddress":
                if (value)
                    value = trimSchema(value);
                PROXYCALLBACK.getManagerAddressCallback(value);
                break;
            case "domain":
                PROXYCALLBACK.getDomainCallback(value);
                break;
            case "companyId":
                PROXYCALLBACK.getCompanyIdCallback(value);
                break;
            case "appGroupList":
                if (value)
                {
                    var ret = JSON.parse(value);
                    if (ret)
                        PROXYCALLBACK.getAppGroupListCallback(ret);
                }
                break;
            case "accountInfo":
                if (value)
                {
                    var ret = JSON.parse(value);
                    if (ret)
                        PROXYCALLBACK.getAccountInfoCallback(ret);
                }
                break;
            case "password":
                PROXYCALLBACK.getPasswordCallback(value);
                break;
            case "lastLoginInfo":
                value = JSON.parse(value);
                if (value && value.domain)
                    value.domain = trimSchema(value.domain);
                PROXYCALLBACK.getLastLoginInfoCallback(value);
                break;
        }
    };

    // 当登录完成后浏览器回调此接口
    var _configDataChanged = function (data, msg) {
		if (msg === "login") {
		    chrome.send("getParam", ["accountInfo"]);
		    chrome.send("getParam", ["appGroupList"]);
		    PROXYCALLBACK.getLoggingStatusCallback(new Number(data));
        } else if (msg === "logout") {
		    PROXYCALLBACK.logOutCallback();
        } else if (msg === "application") {
            if (data) {
                var ret = JSON.parse(data);
                if (ret)
                    PROXYCALLBACK.getAppGroupListCallback(ret);
            }
        } else if (msg === "accountInfo") {
            if (data) {
                var ret = JSON.parse(data);
                if (ret)
                    PROXYCALLBACK.getAccountInfoCallback(ret);
            }
        } else if (msg === "strategy") {
            if (data) {
                var ret = JSON.parse(data);
                if (ret)
                    PROXYCALLBACK.getAppGroupListCallback(ret);
            }
        } else {
            PROXYCALLBACK.getLoggingStatusCallback(new Number(data), msg);
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
} ();
>>>>>>> ffef05f456bc... Add untracked files after rebase
