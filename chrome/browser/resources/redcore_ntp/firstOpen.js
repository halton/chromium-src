/**
 * show first open homepage
 */
function showHello() {
    $('#hello').css('display', 'block');
}

var Hello = {};
window.Hello = Hello;

Hello.showDefaultPage = showHello;