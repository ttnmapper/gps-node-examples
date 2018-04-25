function Decoder(bytes, port) {
  var epoch = (bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0];
  var batvolt = bytes[4];
  var boardtemp = bytes[5];
  var lat = (bytes[9] << 24) | (bytes[8] << 16) | (bytes[7] << 8) | bytes[6];
  var long = (bytes[13] << 24) | (bytes[12] << 16) | (bytes[11] << 8) | bytes[10];
  var alt = (bytes[15] << 8) | bytes[14];
  var speed = (bytes[17] << 8) | bytes[16];
  var course = bytes[18];
  var numsat = bytes[19];
  var fix = bytes[20];
  return {
    epoch: epoch,
    batvolt: batvolt,
    boardtemp: boardtemp,
    lat: lat/10000000.0,
    long: long/10000000.0,
    alt: alt,
    speed: speed,
    course: course,
    numsat: numsat,
    fix: fix,
  };
}
