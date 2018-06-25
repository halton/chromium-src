<<<<<<< HEAD
/**
 * Card.js  首页模块
 * 功能：首页具体实现
 * 描述：
 *     1. 天气卡片
 */

var Card = function () {};
Card.prototype = function () {

    var _localCity = function(){

    }
    // 根据地理位置请求天气信息 -- 和风天气
    // http://www.heweather.com
    // yan.zhang@yunshipei.com
    // z000000
    var _queryWeather = function (location) {
    // 天气卡片存在并且没有请求过天气
        if ($("#weather").length === 1 && $("#weather_temperature").html() === "-°C") {
            $("#weather_location").html(location);
            var weather_api = "https://api.heweather.com/v5/weather?city="+location+"&key=ee62e55b19da4bb489ff359649cd0418";
            $.ajax({
                url: weather_api,
                type: "POST",
                dataType: "json",
                success: function(result) {
                    try {
                        var temp = result["HeWeather5"][0]["aqi"];
                        var data = result["HeWeather5"][0]["now"];
                        var dail = result["HeWeather5"][0]["daily_forecast"];
                        var wind = result["HeWeather5"][0]["hourly_forecast"];
                        if (data != null) {
                            // 设置天气信息
                            $("#weather_weather").html(data.cond.txt);
                            $("#weather_weather_icon").attr("src", "images/weather/" + data.cond.code + ".png");
                            $("#weather_temperature").html(data.tmp+"°C");
                            $('#weather_cond').html(data.cond.txt);
                            $("#weather_qlty").html(temp.city.qlty);
                            $("#weather_temperature_interval").html(dail[0].tmp.max+"/"+dail[0].tmp.min+"°C");
                            $("#weather_wind").html(wind[0].wind.sc);
                        }
                    } catch (error) {
                        console.error(error);
                    }
                },
                error: function() {
                    console.error("请求天气信息出错!");
                }
            });
        }
    }

    //获取当前时间
    var _getNowTime = function(){
        var date = new Date();
        var year = date.getFullYear();
        var month = date.getMonth() + 1;
        var day = date.getDate();
        if (day >= 0 && day <= 9) {
            day = "0" + day;
        }
        var week=["日","一","二","三","四","五","六"];

        var currentDate = year+'年'+month+'月'+day+'日 星期'+ week[new Date().getDay()];
        $('.date').html(currentDate);
    }
    return {
        init: function () {
            _getNowTime();
        },
        quertWeather            : _queryWeather           //天气接口
    };
} ();
||||||| parent of ffef05f456bc... Add untracked files after rebase
=======
/**
 * Card.js  首页模块
 * 功能：首页具体实现
 * 描述：
 *     1. 天气卡片
 */

var Card = function () {};
Card.prototype = function () {

    var _localCity = function(){

    }
    // 根据地理位置请求天气信息 -- 和风天气
    // http://www.heweather.com
    // yan.zhang@yunshipei.com
    // z000000
    var _queryWeather = function (location) {
    // 天气卡片存在并且没有请求过天气
        if ($("#weather").length === 1 && $("#weather_temperature").html() === "-°C") {
            $("#weather_location").html(location);
            var weather_api = "https://api.heweather.com/v5/weather?city="+location+"&key=ee62e55b19da4bb489ff359649cd0418";
            $.ajax({
                url: weather_api,
                type: "POST",
                dataType: "json",
                success: function(result) {
                    try {
                        var temp = result["HeWeather5"][0]["aqi"];
                        var data = result["HeWeather5"][0]["now"];
                        var dail = result["HeWeather5"][0]["daily_forecast"];
                        var wind = result["HeWeather5"][0]["hourly_forecast"];
                        if (data != null) {
                            // 设置天气信息
                            $("#weather_weather").html(data.cond.txt);
                            $("#weather_weather_icon").attr("src", "images/weather/" + data.cond.code + ".png");
                            $("#weather_temperature").html(data.tmp+"°C");
                            $('#weather_cond').html(data.cond.txt);
                            $("#weather_qlty").html(temp.city.qlty);
                            $("#weather_temperature_interval").html(dail[0].tmp.max+"/"+dail[0].tmp.min+"°C");
                            $("#weather_wind").html(wind[0].wind.sc);
                        }
                    } catch (error) {
                        console.error(error);
                    }
                },
                error: function() {
                    console.error("请求天气信息出错!");
                }
            });
        }
    }

    //获取当前时间
    var _getNowTime = function(){
        var date = new Date();
        var year = date.getFullYear();
        var month = date.getMonth() + 1;
        var day = date.getDate();
        if (day >= 0 && day <= 9) {
            day = "0" + day;
        }
        var week=["日","一","二","三","四","五","六"];

        var currentDate = year+'年'+month+'月'+day+'日 星期'+ week[new Date().getDay()];
        $('.date').html(currentDate);
    }
    return {
        init: function () {
            _getNowTime();
        },
        quertWeather            : _queryWeather           //天气接口
    };
} ();
>>>>>>> ffef05f456bc... Add untracked files after rebase
