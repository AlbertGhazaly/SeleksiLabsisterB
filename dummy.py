import hmac
import hashlib
import time

def generate_totp(secret: str, t0: int = 0, time_step: int = 30, hash_algorithm: str = 'sha256') -> str:
    """
    Generate a Time-Based One-Time Password (TOTP) using the provided parameters.
    
    :param secret: The shared secret key as a string.
    :param t0: The initial time (epoch time) as an integer.
    :param time_step: The time step interval in seconds.
    :param hash_algorithm: The hash algorithm to use ('sha256', 'sha1', etc.).
    :return: The TOTP as a string.
    """
    # Convert the secret to bytes (ASCII encoding)
    key = secret.encode()
    
    # Get the current time
    current_time = int(time.time())
    
    # Calculate the time counter
    time_counter = (current_time - t0) // time_step
    
    # Convert the time counter to bytes
    counter_bytes = time_counter.to_bytes(8, byteorder='big')
    
    # Choose the hash function
    if hash_algorithm == 'sha256':
        hash_func = hashlib.sha256
    elif hash_algorithm == 'sha1':
        hash_func = hashlib.sha1
    else:
        raise ValueError('Unsupported hash algorithm')
    
    # Generate the HMAC-SHA hash
    hmac_hash = hmac.new(key, counter_bytes, hash_func).digest()
    
    # Dynamic Truncation
    offset = hmac_hash[-1] & 0x0F
    truncated_hash = int.from_bytes(hmac_hash[offset:offset + 4], byteorder='big') & 0x7FFFFFFF
    
    # Generate the OTP
    otp = truncated_hash % 10**6
    return str(otp).zfill(6)

# Example usage
secret = "tes12345"
# secret = "seleksister2413522150YuukiAsuna"  # Raw ASCII secret key
otp = generate_totp(secret)
print(f"Generated TOTP: {otp}")

# secret = "seleksister2413522150YuukiAsuna"