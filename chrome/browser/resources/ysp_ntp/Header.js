/**
 * Header.js  首页模块
 * 功能：首页具体实现
 * 描述：
 *     1. 显示 -- 显示公司的信息
 *     2. 搜索
 */

var Header = function () {};
Header.prototype = function () {
    // 设置公司信息
    var _setCompanyInfo = function () {
        // 设置图片和公司信息
        if (!DATAJS.iconUrl) {
            $("#company_logo").hide();
            $('#company_logo_default').show();
        } else {
            $('#company_logo_default').hide();
            $("#company_logo").attr("src", DATAJS.iconUrl).show();
        }
        if(!DATAJS.companyTitle){
            $('#company_name').hide()
        } else{
            $('#company_name').text(DATAJS.companyTitle).show();
        }
        if(!JSON.parse(USERINFO).name){
            $('#username').hide()
        } else{
            $('#username').text(JSON.parse(USERINFO).name).show();
        }

    };

    var _bindEvent = function () {
        $("body").on("click", ".logined-search-btn", function(){
            var searchKey =$('.logined-input').val();
            var searchUrl = 'https://www.baidu.com/s?wd='+searchKey;
            window.open(searchUrl);
        });

        $("body").on("click", ".search-btn", function(){
            var searchKey =$('.search-input').val();
            var searchUrl = 'https://www.baidu.com/s?wd='+searchKey;
            window.open(searchUrl);
        });

        $('body').on("keydown", ".notLogined-search-input", function(e){
            var keyValue = e.which;
            if(keyValue == 13){
                $(".search-btn").trigger("click");
            }
        });

        $('body').on("keydown", ".logined-input", function(e){
            var keyValue = e.which;
            if(keyValue == 13){
                $(".logined-search-btn").trigger("click");
            }
        })
    };

    return {
        init: function () {
            // 设置公司信息
            _setCompanyInfo();
        },
        search            : _bindEvent           //搜索
    };
} ();
