import java.lang.reflect.UndeclaredThrowableException;
 import java.security.GeneralSecurityException;
 import java.text.DateFormat;
 import java.text.SimpleDateFormat;
 import java.util.Date;
 import javax.crypto.Mac;
 import javax.crypto.spec.SecretKeySpec;
 import java.math.BigInteger;
 import java.util.TimeZone;


 /**
  * This is an example implementation of the OATH
  * TOTP algorithm.
  * Visit www.openauthentication.org for more information.
  *
  * @author Johan Rydell, PortWise, Inc.
  */

 public class TOTP {

     private TOTP() {}
     public static String stringToHex(String str) {
        StringBuilder hexString = new StringBuilder();
        for (char c : str.toCharArray()) {
            hexString.append(String.format("%02X", (int) c));
        }
        return hexString.toString();
    }
    
     /**
      * This method uses the JCE to provide the crypto algorithm.
      * HMAC computes a Hashed Message Authentication Code with the
      * crypto hash algorithm as a parameter.
      *
      * @param crypto: the crypto algorithm (HmacSHA1, HmacSHA256,
      *                             HmacSHA512)
      * @param keyBytes: the bytes to use for the HMAC key
      * @param text: the message or text to be authenticated
      */
      private static byte[] hmac_sha(String crypto, byte[] keyBytes,
      byte[] text){
  try {
      Mac hmac;
      hmac = Mac.getInstance(crypto);
      SecretKeySpec macKey =
          new SecretKeySpec(keyBytes, "RAW");
      hmac.init(macKey);
      return hmac.doFinal(text);
  } catch (GeneralSecurityException gse) {
      throw new UndeclaredThrowableException(gse);
  }
}


/**
* This method converts a HEX string to Byte[]
*
* @param hex: the HEX string
*
* @return: a byte array
*/

private static byte[] hexStr2Bytes(String hex){
  // Adding one byte to get the right conversion
  // Values starting with "0" can be converted
  byte[] bArray = new BigInteger("10" + hex,16).toByteArray();

  // Copy all the REAL bytes, not the "first"
  byte[] ret = new byte[bArray.length - 1];
  for (int i = 0; i < ret.length; i++)
      ret[i] = bArray[i+1];
  return ret;
}

private static final int[] DIGITS_POWER
// 0 1  2   3    4     5      6       7        8
= {1,10,100,1000,10000,100000,1000000,10000000,100000000 };
 /**
      * This method generates a TOTP value for the given
      * set of parameters.
      *
      * @param key: the shared secret, HEX encoded
      * @param time: a value that reflects a time
      * @param returnDigits: number of digits to return
      *
      * @return: a numeric String in base 10 that includes
      *              {@link truncationDigits} digits
      */

      public static String generateTOTP(String key,
      String time,
      String returnDigits){
  return generateTOTP(key, time, returnDigits, "HmacSHA1");
}


/**
* This method generates a TOTP value for the given
* set of parameters.
*
* @param key: the shared secret, HEX encoded
* @param time: a value that reflects a time
* @param returnDigits: number of digits to return
*
* @return: a numeric String in base 10 that includes
*              {@link truncationDigits} digits
*/

public static String generateTOTP256(String key,
      String time,
      String returnDigits){
  return generateTOTP(key, time, returnDigits, "HmacSHA256");
}
/**
      * This method generates a TOTP value for the given
      * set of parameters.
      *
      * @param key: the shared secret, HEX encoded
      * @param time: a value that reflects a time
      * @param returnDigits: number of digits to return
      *
      * @return: a numeric String in base 10 that includes
      *              {@link truncationDigits} digits
      */

      public static String generateTOTP512(String key,
      String time,
      String returnDigits){
  return generateTOTP(key, time, returnDigits, "HmacSHA512");
}


/**
* This method generates a TOTP value for the given
* set of parameters.
*
* @param key: the shared secret, HEX encoded
* @param time: a value that reflects a time
* @param returnDigits: number of digits to return
* @param crypto: the crypto function to use
*
* @return: a numeric String in base 10 that includes
*              {@link truncationDigits} digits
*/

public static String generateTOTP(String key,
      String time,
      String returnDigits,
      String crypto){
  int codeDigits = Integer.decode(returnDigits).intValue();
  String result = null;

  // Using the counter
  // First 8 bytes are for the movingFactor
  // Compliant with base RFC 4226 (HOTP)
  while (time.length() < 16 )
      time = "0" + time;

  // Get the HEX in a Byte[]
  byte[] msg = hexStr2Bytes(time);
  byte[] k = hexStr2Bytes(key);
  byte[] hash = hmac_sha(crypto, k, msg);

         // put selected bytes into result int
         int offset = hash[hash.length - 1] & 0xf;

         int binary =
             ((hash[offset] & 0x7f) << 24) |
             ((hash[offset + 1] & 0xff) << 16) |
             ((hash[offset + 2] & 0xff) << 8) |
             (hash[offset + 3] & 0xff);

         int otp = binary % DIGITS_POWER[codeDigits];

         result = Integer.toString(otp);
         while (result.length() < codeDigits) {
             result = "0" + result;
         }
         return result;
     }

     public static void main(String[] args) {
        // Define parameters
        String key = stringToHex("tes12345"); // Replace this with your actual HEX encoded key
        long t0 = 0; // Initial timestamp (T0)
        long timeStep = 30; // Time step in seconds
        String returnDigits = "8"; // Number of digits to return
        
        // Calculate the current timestamp
        long currentTime = System.currentTimeMillis() / 1000; // Get current time in seconds
        long counter = (currentTime - t0) / timeStep; // Calculate the counter value
        
        // Convert the counter to HEX string (8 bytes as a fixed size)
        String timeHex = String.format("%016X", counter);
        
        // Generate TOTP using SHA-256
        String otp = generateTOTP256(key, timeHex, returnDigits);
        
        // Print the result
        System.out.println("Generated TOTP: " + otp);
    }
}