/**
 * ShowBrowser.js  与浏览器交互的功能js
 */

var ShowBrowser = function () {};
ShowBrowser.prototype = function () {

    // 浏览器的获取参数完成的回调方法
    var _getParamFinish = function (key, value) {
        // 根据key去分别调用不同模块的业务方法
        switch (key) {
            // 获取客户端登录状态
            case "loggingStatus":
                SHOWDATA.getLoggingStatusFinish(value);
                break;
            // 获取config.js文件
            case "ShowConfig":
                SHOWDATA.getShowConfigFinish(value);
                break;

        }
    };
    var _loginChanged = function () {
        chrome.send("getParam", ["loggingStatus"]);
    };
    return {
        getParamFinish          : _getParamFinish,        // 浏览器的获取参数完成的回调方法
        loginChanged            : _loginChanged,
    };
} ();
