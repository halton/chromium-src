/**
 * Server.js  与服务器端交互的功能js
 */

var Server = function () {};
Server.prototype = function () {

    // 根据域名获取服务器地址
    var _getAddressByDomain = function (data) {
        var address = "https://hub.enterplorer.com/api/companies/manager/url/";
        // 判断下域名是否有ysptest-或www.ysptest-
        if (data.domain.split("ysptest-").length > 1  || data.domain.split("www.ysptest-").length > 1) {
            address = "https://hubtest.enterplorer.com/api/companies/manager/url/";
        }

        $.ajax({
            async: true,
            crossDomain: true,
            type : "GET",
            url: address + data.domain,
            success: function(response) {
                if (response.manager_url !== undefined) {
                    if($.isFunction(data.callback)){
                        data.callback(response.manager_url);
                    }
                } else {
                    alert("获取服务器地址错误");
                }
            }
        });
    };


    // 获取公司信息
    var _getCompanyInfo = function (data) {
        $.ajax({
            async: true,
            crossDomain: true,
            type : "POST",
            url: data.serverAddress+"/v2/companyInfo",
            data: {
                "cid": data.companyDomain
            },
            success: function(response) {
                if($.isFunction(data.callback)){
                    data.callback(response);
                }
            }
        });
    };

    // 登录
    var _login = function (data) {
        var userName      = data.userName,
            password      = data.password,
            deviceInfo    = data.deviceInfo,
            serverAddress = data.serverAddress,
            companyId     = data.companyId,
            callback      = data.callback;

        $.ajax({
            async: true,
            crossDomain: true,
            type : "POST",
            url: data.serverAddress+"/v2/newlogin",
            data: {
                "companyId"  : data.companyId,
                "uuid"       : data.userName,
                "password"   : data.password,
                "timestamp"  : "0",
                "deviceInfo" : data.deviceInfo
            },
            success: function(response) {
                if (response.status === 1) {
                    if($.isFunction(data.callback)){
                        data.callback(response.data);
                    }
                } else {
                    alert(response.message);
                }
            }
        });
    };

    return {
        getAddressByDomain : _getAddressByDomain, // 根据域名获取服务器地址
        getCompanyInfo     : _getCompanyInfo,     // 根据用户id获取数据
        login              : _login               // 登录
    };
} ();
