import requests
from requests.auth import HTTPBasicAuth
import base64
import hmac
import hashlib
import time

# Define the digits power array
DIGITS_POWER = [1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000]

def hmac_sha(crypto, key_bytes, text):
    """
    Computes the HMAC using the specified hash algorithm.

    :param crypto: the crypto algorithm ('sha1', 'sha256', 'sha512')
    :param key_bytes: the bytes to use for the HMAC key
    :param text: the message or text to be authenticated
    :return: the HMAC digest
    """
    hash_func = {
        'sha1': hashlib.sha1,
        'sha256': hashlib.sha256,
        'sha512': hashlib.sha512
    }.get(crypto)
    
    if hash_func is None:
        raise ValueError('Unsupported hash algorithm')
    
    hmac_obj = hmac.new(key_bytes, text, hash_func)
    return hmac_obj.digest()

def generate_totp(secret, t0=0, time_step=30, hash_algorithm='sha256', return_digits=6):
    """
    Generates a TOTP value for the given parameters.

    :param secret: the shared secret as a string
    :param t0: the initial time (epoch time) as an integer
    :param time_step: the time step interval in seconds
    :param hash_algorithm: the hash algorithm to use ('sha1', 'sha256', 'sha512')
    :param return_digits: number of digits to return
    :return: a numeric string in base 10 that includes return_digits digits
    """
    # Convert the secret to bytes (UTF-8 encoding)
    key_bytes = secret.encode('utf-8')
    
    # Get the current time
    current_time = int(time.time())
    
    # Calculate the time counter
    time_counter = (current_time - t0) // time_step
    
    # Convert the time counter to bytes
    counter_bytes = time_counter.to_bytes(8, byteorder='big')
    
    # Generate the HMAC-SHA hash
    hash_bytes = hmac_sha(hash_algorithm, key_bytes, counter_bytes)
    
    # Dynamic Truncation
    offset = hash_bytes[-1] & 0x0F
    binary = (
        ((hash_bytes[offset] & 0x7F) << 24) |
        ((hash_bytes[offset + 1] & 0xFF) << 16) |
        ((hash_bytes[offset + 2] & 0xFF) << 8) |
        (hash_bytes[offset + 3] & 0xFF)
    )
    
    otp = binary % DIGITS_POWER[return_digits]
    result = str(otp).zfill(return_digits)
    
    return result
# Example usage
if __name__ == "__main__":
    # secret = "seleksister2413522150YuukiAsuna"  # Your secret string
    secret = "seleksister2413522150YuukiAsuna"
    t0 = 0
    time_step = 30
    hash_algorithm = 'sha256'
    return_digits = 8

    otp = generate_totp(secret, t0, time_step, hash_algorithm, return_digits)
    print(f"Generated TOTP: {otp}")

    # Konfigurasi
    userid = '13522150'
    password = otp  # Ini akan diganti dengan OTP yang dihasilkan
    url = 'http://sister21.tech:7787/recruitment/submit/a'
    
    # Generate OTP
    # Buat objek TOTP dengan secret yang sesuai
    # Gunakan secret yang sama dengan yang digunakan untuk generate OTP di aplikasi asli

    # Ganti password dengan OTP yang dihasilkan
    auth_password = otp

    # Encode userid dan password menjadi format Base64 untuk Authorization header
    credentials = f"{userid}:{auth_password}"
    encoded_credentials = base64.b64encode(credentials.encode('utf-8')).decode('utf-8')
    headers = {
        'Authorization': f'Basic {encoded_credentials}',
        'Content-Type': 'application/json'
    }
    data = {
        "fullname": "Albert Ghazaly",
        "link": "https://drive.google.com/file/d/1CgHnSxHtmLfXuf-HdYFU7b7LOaLsfffT/view?usp=sharing",
        "message": "Mohon bantu saya mencari portal menuju isekai"
    }
    # Kirimkan request POST
    response = requests.post(url, headers=headers, json=data)

    # Cek hasil request
    if response.status_code == 200:
        print("Request berhasil")
        print(response.status_code)
        print(response.text)
    else:
        print(f"Request gagal dengan status code: {response.status_code}")
        print(f"Pesan error: {response.text}")

