/**
 * generate Header
 * @param {object} data: title, logo, banner
 */
function generateHeader(data) {
  var defaultData ={
    title: data.title || '',
    logo: data.logo || './imgs/defaultLogo.png',
  }

  $('#title').text(defaultData.title);
  $('#logo').attr('src', defaultData.logo);
}



var Header = {};
window.Header = Header;

Header.generate = generateHeader;