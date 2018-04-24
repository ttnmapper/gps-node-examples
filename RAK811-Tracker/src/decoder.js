function Decoder(b, port) {
var decoded = {};
    switch (port) {
        case 2:
            var lat = (b[0] | b[1]<<8 | b[2]<<16 | (b[2] & 0x80 ? 0xFF<<24 : 0)) / 10000.0;
            var lon = (b[3] | b[4]<<8 | b[5]<<16 | (b[5] & 0x80 ? 0xFF<<24 : 0)) / 10000.0;
            var alt = (b[6] | b[7]<<8 | (b[7] & 0x80 ? 0xFF<<16 : 0)) / 100.0;
            var hdop = b[8] / 10.0;
        
            decoded = {
            "location": {
                "lat": lat,
                "lon": lon,
                "alt": alt,
                "hdop": hdop
            },
        };
        break;
        
        case 3:
        var bat = (b[0] | b[1]<<8 | (b[1] & 0x80 ? 0xFF<<16 : 0)) / 10.0;

        decoded ={
        "battery":{
            "bat":bat
        }    
        };
        break;
    
        case 4:
        var aX = (b[0] | b[1]<<8 | (b[1] & 0x80 ? 0xFF<<16 : 0)) / 10.0;
        var aY = (b[2] | b[3]<<8 | (b[3] & 0x80 ? 0xFF<<16 : 0)) / 10.0;
        var aZ = (b[4] | b[5]<<8 | (b[5] & 0x80 ? 0xFF<<16 : 0)) / 10.0;

        decoded ={
        "acceleration":{
            "aX":aX,
            "aY":aY,
            "aZ":aZ,
        }    
        };
        break;
}

return decoded;
}