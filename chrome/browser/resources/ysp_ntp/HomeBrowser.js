<<<<<<< HEAD
/**
 * HomeBrowser.js  与浏览器交互的功能js
 */

var HomeBrowser = function () {};
HomeBrowser.prototype = function () {

    // 浏览器的设置参数完成的回调方法
    var _setParamFinish = function (key) {
        // 根据key去分别调用不同模块的业务方法
        switch (key) {
            case "loggingStatus":
                HOMEDATA.setLoggingStatusFinish();
                break;
            case "autoLogin":
                HOMEDATA.setAutoLoginFinish();
                break;
            case "address":
                HOMEDATA.setAddressFinish();
                break;
            case "domain":
                HOMEDATA.setDomainFinish();
                break;
            case "companyId":
                HOMEDATA.setCompanyIdFinish();
                break;
            case "userInfo":
                HOMEDATA.setUserInfoFinish();
                break;
            case "password":
                HOMEDATA.setPasswordFinish();
                break;
            case "dataJS":
                HOMEDATA.setDataJSFinish();
                break;
        }
    };

    // 浏览器的获取参数完成的回调方法
    var _getParamFinish = function (key, value) {
        // 根据key去分别调用不同模块的业务方法
        switch (key) {
            case "loggingStatus":
                HOMEDATA.getLoggingStatusFinish(value);
                break;
            case "autoLogin":
                HOMEDATA.getAutoLoginFinish(value);
                break;
            case "address":
                HOMEDATA.getAddressFinish(value);
                break;
            case "domain":
                HOMEDATA.getDomainFinish(value);
                break;
            case "companyId":
                HOMEDATA.getCompanyIdFinish(value);
                break;
            case "dataJS":
                HOMEDATA.getDataJSFinish(value);
                break;
            case "userInfo":
                HOMEDATA.getUserInfoFinish(value);
                break;
            case "password":
                HOMEDATA.getPasswordFinish(value);
                break;

        }
    };

    // 获得设备信息的回调方法
    var _getDeviceInfoFinish = function (deviceInfo) {
        HOMEDATA.getDeviceInfoFinish(deviceInfo);
    };

    // 当登录完成后浏览器回调此接口
    var _refreshDataJS = function (key) {
		if (key === "login") {
			chrome.send("getParam", ["dataJS"]);
		} else if (key === "logout") {
            HOMEDATA.logout();
        } else if (key === "forgotPassword") {
            window.location.href = "chrome://ep-fpwd/";
        }
    };

    //获取当前城市
    var _getCityFinish = function(data){
        // data ='{"city":"北京","country":"中国","desc":"","district":"","end":-1,"isp":"","province":"北京","ret":1,"start":-1,"type":""}';
        CARD.quertWeather(JSON.parse(data).city);
    }

    //登录状态改变后浏览器端调用
    var _loginChanged = function(){
        chrome.send("getParam", ["loggingStatus"]);
    }

    return {
        setParamFinish          : _setParamFinish,        // 浏览器的设置参数完成的回调方法
        getParamFinish          : _getParamFinish,        // 浏览器的获取参数完成的回调方法
        getDeviceInfoFinish     : _getDeviceInfoFinish,   // 获得设备信息的回调方法
        refreshDataJS           : _refreshDataJS,         // 当登录完成后浏览器回调此接口
        getCityFinish           : _getCityFinish,         // 当浏览器获取当前城市后回调此接口
        loginChanged            : _loginChanged           // 当浏览器登录状态切换后调用此接口
    };
} ();
||||||| parent of ffef05f456bc... Add untracked files after rebase
=======
/**
 * HomeBrowser.js  与浏览器交互的功能js
 */

var HomeBrowser = function () {};
HomeBrowser.prototype = function () {

    // 浏览器的设置参数完成的回调方法
    var _setParamFinish = function (key) {
        // 根据key去分别调用不同模块的业务方法
        switch (key) {
            case "loggingStatus":
                HOMEDATA.setLoggingStatusFinish();
                break;
            case "autoLogin":
                HOMEDATA.setAutoLoginFinish();
                break;
            case "address":
                HOMEDATA.setAddressFinish();
                break;
            case "domain":
                HOMEDATA.setDomainFinish();
                break;
            case "companyId":
                HOMEDATA.setCompanyIdFinish();
                break;
            case "userInfo":
                HOMEDATA.setUserInfoFinish();
                break;
            case "password":
                HOMEDATA.setPasswordFinish();
                break;
            case "dataJS":
                HOMEDATA.setDataJSFinish();
                break;
        }
    };

    // 浏览器的获取参数完成的回调方法
    var _getParamFinish = function (key, value) {
        // 根据key去分别调用不同模块的业务方法
        switch (key) {
            case "loggingStatus":
                HOMEDATA.getLoggingStatusFinish(value);
                break;
            case "autoLogin":
                HOMEDATA.getAutoLoginFinish(value);
                break;
            case "address":
                HOMEDATA.getAddressFinish(value);
                break;
            case "domain":
                HOMEDATA.getDomainFinish(value);
                break;
            case "companyId":
                HOMEDATA.getCompanyIdFinish(value);
                break;
            case "dataJS":
                HOMEDATA.getDataJSFinish(value);
                break;
            case "userInfo":
                HOMEDATA.getUserInfoFinish(value);
                break;
            case "password":
                HOMEDATA.getPasswordFinish(value);
                break;

        }
    };

    // 获得设备信息的回调方法
    var _getDeviceInfoFinish = function (deviceInfo) {
        HOMEDATA.getDeviceInfoFinish(deviceInfo);
    };

    // 当登录完成后浏览器回调此接口
    var _refreshDataJS = function (key) {
		if (key === "login") {
			chrome.send("getParam", ["dataJS"]);
		} else if (key === "logout") {
            HOMEDATA.logout();
        } else if (key === "forgotPassword") {
            window.location.href = "chrome://ep-fpwd/";
        }
    };

    //获取当前城市
    var _getCityFinish = function(data){
        // data ='{"city":"北京","country":"中国","desc":"","district":"","end":-1,"isp":"","province":"北京","ret":1,"start":-1,"type":""}';
        CARD.quertWeather(JSON.parse(data).city);
    }

    //登录状态改变后浏览器端调用
    var _loginChanged = function(){
        chrome.send("getParam", ["loggingStatus"]);
    }

    return {
        setParamFinish          : _setParamFinish,        // 浏览器的设置参数完成的回调方法
        getParamFinish          : _getParamFinish,        // 浏览器的获取参数完成的回调方法
        getDeviceInfoFinish     : _getDeviceInfoFinish,   // 获得设备信息的回调方法
        refreshDataJS           : _refreshDataJS,         // 当登录完成后浏览器回调此接口
        getCityFinish           : _getCityFinish,         // 当浏览器获取当前城市后回调此接口
        loginChanged            : _loginChanged           // 当浏览器登录状态切换后调用此接口
    };
} ();
>>>>>>> ffef05f456bc... Add untracked files after rebase
