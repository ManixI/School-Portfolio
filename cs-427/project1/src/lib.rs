#![allow(dead_code, unused)]

use rand::Rng;
use std::fs::File;
use std::io::prelude::*;
use std::io;
use std::num::Wrapping;

static F_TABLE: [u8; 256] = [
    0xa3,0xd7,0x09,0x83,0xf8,0x48,0xf6,0xf4,0xb3, 0x21,0x15,0x78,0x99,0xb1,0xaf,0xf9, 
    0xe7,0x2d,0x4d,0x8a,0xce,0x4c,0xca,0x2e,0x52,0x95,0xd9,0x1e,0x4e,0x38,0x44,0x28, 
    0x0a,0xdf,0x02,0xa0,0x17,0xf1,0x60,0x68,0x12,0xb7,0x7a,0xc3,0xe9,0xfa,0x3d,0x53, 
    0x96,0x84,0x6b,0xba,0xf2,0x63,0x9a,0x19,0x7c,0xae,0xe5,0xf5,0xf7,0x16,0x6a,0xa2, 
    0x39,0xb6,0x7b,0x0f,0xc1,0x93,0x81,0x1b,0xee,0xb4,0x1a,0xea,0xd0,0x91,0x2f,0xb8, 
    0x55,0xb9,0xda,0x85,0x3f,0x41,0xbf,0xe0,0x5a,0x58,0x80,0x5f,0x66,0x0b,0xd8,0x90, 
    0x35,0xd5,0xc0,0xa7,0x33,0x06,0x65,0x69,0x45,0x00,0x94,0x56,0x6d,0x98,0x9b,0x76, 
    0x97,0xfc,0xb2,0xc2,0xb0,0xfe,0xdb,0x20,0xe1,0xeb,0xd6,0xe4,0xdd,0x47,0x4a,0x1d, 
    0x42,0xed,0x9e,0x6e,0x49,0x3c,0xcd,0x43,0x27,0xd2,0x07,0xd4,0xde,0xc7,0x67,0x18, 
    0x89,0xcb,0x30,0x1f,0x8d,0xc6,0x8f,0xaa,0xc8,0x74,0xdc,0xc9,0x5d,0x5c,0x31,0xa4, 
    0x70,0x88,0x61,0x2c,0x9f,0x0d,0x2b,0x87,0x50,0x82,0x54,0x64,0x26,0x7d,0x03,0x40, 
    0x34,0x4b,0x1c,0x73,0xd1,0xc4,0xfd,0x3b,0xcc,0xfb,0x7f,0xab,0xe6,0x3e,0x5b,0xa5, 
    0xad,0x04,0x23,0x9c,0x14,0x51,0x22,0xf0,0x29,0x79,0x71,0x7e,0xff,0x8c,0x0e,0xe2, 
    0x0c,0xef,0xbc,0x72,0x75,0x6f,0x37,0xa1,0xec,0xd3,0x8e,0x62,0x8b,0x86,0x10,0xe8, 
    0x08,0x77,0x11,0xbe,0x92,0x4f,0x24,0xc5,0x32,0x36,0x9d,0xcf,0xf3,0xa6,0xbb,0xac, 
    0x5e,0x6c,0xa9,0x13,0x57,0x25,0xb5,0xe3,0xbd,0xa8,0x3a,0x01,0x05,0x59,0x2a,0x46
    ];

pub fn read_keyfile(file: &str) -> [u8; 8] {

    let mut contents = String::with_capacity(16);
    let mut file = File::open(file).unwrap();
    file.read_to_string(&mut contents).unwrap();

    let bytes = contents.as_bytes();
    let mut out: [u8; 8] = [0; 8];
    for i in 0..16 {
        // convert ascii byte to int value
        out[i/2] |= match bytes[i] {
            0x30 => 0x0,
            0x31 => 0x1,
            0x32 => 0x2,
            0x33 => 0x3,
            0x34 => 0x4,
            0x35 => 0x5,
            0x36 => 0x6,
            0x37 => 0x7,
            0x38 => 0x8,
            0x39 => 0x9,
            0x61 => 0xa,
            0x62 => 0xb,
            0x63 => 0xc,
            0x64 => 0xd,
            0x65 => 0xe,
            0x66 => 0xf,
            _ => panic!("Error: key contains bad byte. Hint: first 16 chars must be either 0-9 or a-f case sensitive."),
        };
        if i % 2 == 0 {
            // shift left if upper half of byte
            out[i/2] <<= 4;
        }
    }
    out
}

pub fn encrypt_file(file: &str, key: &mut [u8; 8], outfile: &str, encrypt: bool) -> io::Result<()> {
    let mut compressed_key: [u16; 4] = [0; 4];
    
    let mut input = File::open(file)?;
    let mut eof = false;

    let mut outfile = File::create(outfile)?;

    let mut w: [u8; 8];
    let mut file_bytes: io::Bytes<File> = input.bytes();

    while !eof {
        for i in 0..4 {
            compressed_key[i] = concat_u8s(key[i*2], key[i*2+1]);
        }
        // for debug only, normally don't treat ascii in file as hex value of written vals
        w = read_keyfile(file);
        let mut wp = [0; 4];
        for i in 0..4 {
            wp[i] = concat_u8s(w[i*2], w[i*2+1]);
        }
        let mut w = wp;

        // do whitening
        let mut r = whitening(&compressed_key, &w);

        // do rounding
        for rnd in 0..16 {
            r = round(r, key, rnd, encrypt);
        }
        // last step for encryption
        let y = [
            r[2], r[3], r[0], r[1]
        ];
        for i in 0..4 {
            compressed_key[i] = concat_u8s(key[i*2], key[i*2+1]);
        }

        let out = whitening(&compressed_key, &y);
        let mut out_bytes: [u8; 8] = [0; 8];
        for i in 0..8 {
            if i%2==0 {
                out_bytes[i] = (out[i/2] >> 8) as u8;
            } else {
                out_bytes[i] = out[i/2] as u8;
            }
        }
        outfile.write_all(&out_bytes);
        // for test only
        eof = true;
    }
    Ok(())
}

// handle decryption/encryption difference by passing flag to g/f functions
// to choose which key schedule to call

pub fn decrypt_file(file: &str, key: &mut [u8; 8], outfile: &str) -> io::Result<()> {
    let mut compressed_key: [u16; 4] = [0; 4];
    
    let mut input = File::open(file)?;
    let mut eof = false;

    let mut outfile = File::create(outfile)?;

    let mut w: [u8; 8];
    let mut file_bytes: io::Bytes<File> = input.bytes();

    while !eof {
        for i in 0..4 {
            compressed_key[i] = concat_u8s(key[i*2], key[i*2+1]);
        }
        // get values of w0-w3, shift them into array
        // for debug only
        w = read_keyfile(file);
        let mut wp = [0; 4];
        for i in 0..4 {
            wp[i] = concat_u8s(w[i*2], w[i*2+1]);
        }
        let mut w = wp;

        // do whitening
        let mut r = whitening(&compressed_key, &w);

        // do rounding
        for rnd in 0..16 {
            r = round(r, key, rnd, false);
        }
        // last step for encryption
        let y = [
            r[2], r[3], r[0], r[1]
        ];
        for i in 0..4 {
            compressed_key[i] = concat_u8s(key[i*2], key[i*2+1]);
        }

        let out = whitening(&compressed_key, &y);
        let mut out_bytes: [u8; 8] = [0; 8];
        for i in 0..8 {
            if i%2==0 {
                out_bytes[i] = (out[i/2] >> 8) as u8;
            } else {
                out_bytes[i] = out[i/2] as u8;
            }
        }
        outfile.write_all(&out_bytes);
        // for test only
        eof = true;
    }


    Ok(())
}

// decrypt round func
fn round(r: [u16; 4], key: &mut [u8; 8], round: usize, encrypt: bool) -> [u16; 4] {
    let (f0, f1) = f_encrypt(r[0], r[1], round, key, encrypt);
        let mut new_r: [u16; 4] = [0; 4];
    
        if !encrypt {
            new_r[0] = r[2].rotate_left(1) ^ f0;
            new_r[1] = (r[3] ^ f1).rotate_right(1);
        } else {
            new_r[0] = (r[2] ^ f0).rotate_right(1);
            new_r[1] = (r[3].rotate_left(1)) ^ f1;
        }
        new_r[2] = r[0];
        new_r[3] = r[1];
        new_r
}

fn get_8_bytes(input: &mut io::Bytes<File>) -> ([u16; 4], bool) {
    let mut out: [u16; 4] = [0; 4];
    let mut eof = false;
    for i in 0..8 {
        let val = input.next();
        if val.is_none() {
            eof = true;
            for j in i..8 {
                out[j/2] = 0;
            }
            break;
        }
        let val = val.unwrap().unwrap();
        out[i/2] |= val as u16;
        if i % 2 == 0 {
            out[i/2] <<= 8;
        }
    }

    (out, eof)
}

fn whitening(key: &[u16; 4], in_block: &[u16; 4]) -> [u16; 4] {
    let mut out: [u16; 4] = [0; 4];
    for i in 0..4 {
        out[i] = key[i] ^ in_block[i];
    }
    out
}

fn f_encrypt(r0: u16, r1: u16, round: usize, key: &mut [u8; 8], encrypt: bool) -> (u16, u16) {
    let mut subkeys: Vec<u8> = Vec::<u8>::with_capacity(12);
    for i in 0..12 {
        subkeys.push(key_schedule(&(4*round +(i%4)), key, encrypt));
    }
    if !encrypt {
        //subkeys = subkeys.into_iter().rev().collect(); // not needed?
    }

    let t0 = Wrapping(g_permutation(r0, round, key, encrypt, &subkeys[0..=3]));
    let t1 = Wrapping(g_permutation(r1, round, key, encrypt, &subkeys[4..=7]));
    // numbers too small, don't need mod, just let it overflow
    let f0 = t0 + Wrapping(2)*t1 + Wrapping(concat_u8s(subkeys[8], subkeys[9]));
    let f1 = Wrapping(2)*t0 + t1 + Wrapping(concat_u8s(subkeys[10], subkeys[11]));
    
    (f0.0, f1.0)
    
}

// a is the left, b is the right
fn concat_u8s(a: u8, b: u8) -> u16 {
    let mut out = a as u16;
    out <<= 8;
    out | b as u16
}

fn g_permutation(w: u16, round: usize, key: &mut [u8; 8], encrypt: bool, subkeys: &[u8]) -> u16 {
    let g1: u8 = (w >> 8) as u8;
    let g2: u8 = w  as u8;
    //if encrypt {
        let g3: u8 = F_TABLE[(g2 ^ subkeys[0]) as usize] ^ g1;
        let g4: u8 = F_TABLE[(g3 ^ subkeys[1]) as usize] ^ g2;
        let g5: u8 = F_TABLE[(g4 ^ subkeys[2]) as usize] ^ g3;
        let g6: u8 = F_TABLE[(g5 ^ subkeys[3]) as usize] ^ g4;
        println!("g: {} {} {} {} {} {}", g1, g2, g3, g4, g5, g6);
        concat_u8s(g5, g6)
    /* } else {
        let g3: u8 = F_TABLE[(g2 ^ key_schedule(&(4*round+3), key, encrypt)) as usize] ^ g1;
        let g4: u8 = F_TABLE[(g3 ^ key_schedule(&(4*round+2), key, encrypt)) as usize] ^ g2;
        let g5: u8 = F_TABLE[(g4 ^ key_schedule(&(4*round+1), key, encrypt)) as usize] ^ g3;
        let g6: u8 = F_TABLE[(g5 ^ key_schedule(&(4*round),   key, encrypt)) as usize] ^ g4;
        
        concat_u8s(g5, g6)
    }*/
}

fn compress_key_u64(key: &[u8; 8]) -> u64 {
    let mut out = key[0] as u64;
    for val in key.iter().skip(1) {
        out <<= 8;
        out |= *val as u64;
    }
    out
}

fn split_key_8(key: u64, new_key: &mut [u8; 8]) {
    let mut key = key;
    for i in (0..8).rev() {
        new_key[i] = key as u8;        // this should truncate just fine
        key >>= 8;
    } 
}

fn key_schedule(x: &usize, key: &mut [u8; 8], encrypt: bool) -> u8 {
    // convert arr of u8s to single u64
    let mut full_key: u64 = compress_key_u64(key);
    
    if encrypt {
        // rotate left 1
        full_key = full_key.rotate_left(1);

        // split new key back into bytes
        split_key_8(full_key, key);

        key[7-(x%8)]
    } else {
        let out = key[(x%8)];
        println!("key: {}", full_key);
        full_key = full_key.rotate_right(1);
        println!("key: {}, out: {}", full_key, out);
        split_key_8(full_key, key);
        out
    }   
}

// a^b mod n
pub fn exp(mut a: u128, mut b: u128, n: u128) -> u128 {
    let mut total = 1;

    while b > 0 {
        if (b & 0b1) == 0b1 {
            total *= a;
            total %= n;
        }
        a *= a;
        a %= n;
        b >>= 1;
    }

    total % n
}

/**
 * n: num to check
 * a: exponent
 */
pub fn miller_rabin(n: u128, mut a: u128) -> bool {
    let r: u128 = rand::thread_rng().gen_range(2..=(n-2));
    let mut p = exp(r, a, n);

    if p == 1 || p == n-1 {
        return true;
    }

    while a != n-1 {
        p = (p*p) % n;
        a *= 2; // overflow error here

        if p == 1 {
            return false;
        }
        if p == n-1 {
            return true;
        }
    }
    false
}

/**
 * n: number
 * a: num itterations
 */
fn is_prime(n: u128, a: u128) -> bool {
    if n<=1 || n==4 {
        return false;
    }
    if n<=3 {
        return true;
    }

    let mut d = n-1;
    while d % 2 == 0 {
        d /= 2;
    }

    for _ in 0..a {
        if !miller_rabin(n, d) {
            return false;
        }
    }
    true
}

#[cfg(test)]
mod tests {
    use std::ptr::read;

    use crate::*;

    static FILE: &str = "docs/plaintext.txt";

    static TEST_KEY: [u8; 8] = [
        0xab, 0xcd, 0xef, 0x01,
        0x23, 0x45, 0x67, 0x89
    ];

    static TEST_KEY_COMPRESSED: [u16; 4] = [
        0xabcd, 0xef01,
        0x2345, 0x6789
    ];

    static TEST_BLOCK: [u16; 4] = [
        0x0123, 0x4567,
        0x89ab, 0xcdef
    ];

    static TEST_BLOCK_E: [u16; 4] = [
        0x9a76, 0xd6d5,
        0x78c4, 0x4766
    ];

    static TEST_ENCRYPTED_BLOCK: u64 = 0x9a76d6d578c44766;

    static TEST_DECRYPTED_BLOCK: u64 = 0x0123456789abcdef;

    #[test]
    fn test_round() {
        let mut key = read_keyfile("docs/testkey.txt");
        let mut compressed_key = [0;4];
        for i in 0..4 {
            compressed_key[i] = concat_u8s(key[i*2], key[i*2+1]);
        } 
        let r = whitening(&mut compressed_key, &TEST_BLOCK);
        assert_eq!(round(r, &mut key, 0, true), [
            0xca2f, 0xf024, 0xaaee, 0xaa66
        ]);

        let mut key = read_keyfile("docs/testkey.txt");
        let mut compressed_key = [0;4];
        for i in 0..4 {
            compressed_key[i] = concat_u8s(key[i*2], key[i*2+1]);
        } 
        let r = whitening(&mut compressed_key, &TEST_BLOCK_E);
        assert_eq!(round(r, &mut key, 0, false), [
            0x0abe, 0xcecf, 0x31bb, 0x39d4
        ]);
    }

    #[test]
    fn test_encrypt() {
        let mut key = read_keyfile("docs/testkey.txt");
        encrypt_file("docs/testinput.txt", &mut key, "docs/testout.txt", true);
        
        let mut contents = [0; 8];
        let mut file = File::open("docs/testout.txt").unwrap();
        file.read_exact(&mut contents).unwrap();
        
        let mut out: u64 = 0;
        for byte in contents {
            out <<= 8;
            out |= byte as u64;
        }
        assert_eq!(out, TEST_ENCRYPTED_BLOCK);
    }

    //#[test]
    fn test_decrypt() {
        let mut key = read_keyfile("docs/testkey.txt");
        encrypt_file("docs/testdecrypt.txt", &mut key, "docs/testout.txt", false);
        
        let mut contents = [0; 8];
        let mut file = File::open("docs/testout.txt").unwrap();
        file.read_exact(&mut contents).unwrap();
        
        let mut out: u64 = 0;
        for byte in contents {
            out <<= 8;
            out |= byte as u64;
        }
        assert_eq!(out, TEST_DECRYPTED_BLOCK);
    }

    #[test]
    fn test_exponent() {
        assert_eq!(exp(2, 2, 2), (2*2)%2);
        assert_eq!(exp(2, 2, 3), (2*2)%3);
        assert_eq!(exp(2, 10, 2), 0);
        assert_eq!(exp(3, 2, 2), (3*3)%2);
        assert_eq!(exp(3, 2, 5), (3*3)%5);
        assert_eq!(exp(3, 3, 5), (3*3*3)%5);
    }

    #[test]
    fn test_is_prime() {
        assert_eq!(is_prime(2, 4), true);
        assert_eq!(is_prime(3, 4), true);
        assert_eq!(is_prime(5, 4), true);
        assert_eq!(is_prime(31, 4), true);
        assert_eq!(is_prime(59, 4), true);
        assert_eq!(is_prime(97, 4), true);
        assert_eq!(is_prime(271, 4), true);
        assert_eq!(is_prime(7919, 4), true);

        assert_eq!(is_prime(1, 4), false);
        assert_eq!(is_prime(4, 4), false);
        assert_eq!(is_prime(22, 4), false);
        assert_eq!(is_prime(100, 4), false);
        assert_eq!(is_prime(123456789, 4), false);
        assert_eq!(is_prime(100000000000, 4), false);
    }

    #[test]
    fn test_concat() {
        assert_eq!(concat_u8s(0xaa, 0xbb), 0xaabb);
        assert_eq!(concat_u8s(0xff, 0xff), 0xffff);
        assert_eq!(concat_u8s(0x00, 0x00), 0x0000);
    }

    #[test]
    fn test_read_keyfile() {
        assert_eq!(read_keyfile("docs/key.txt"), TEST_KEY);
    }

    #[test]
    fn test_whitening() {
        assert_eq!(whitening(&TEST_KEY_COMPRESSED, &TEST_BLOCK),
        [0xaaee, 0xaa66, 0xaaee, 0xaa66]);

        assert_eq!((whitening(&TEST_KEY_COMPRESSED, &TEST_BLOCK_E)),
        [0x31bb, 0x39d4, 0x5b81, 0x20ef]);
    }

    // dose not test EOF
    #[test]
    fn test_get_8_bytes() {
        let mut input = File::open(FILE).unwrap();
        let mut file_bytes: io::Bytes<File> = input.bytes();
        assert_eq!(get_8_bytes(&mut file_bytes), ([
            0x4265, 0x666f,
            0x7265, 0x2074
        ], false));

    }

    #[test]
    fn test_compress_key_u64() {
        assert_eq!(compress_key_u64(&TEST_KEY), 0xabcdef0123456789);
    }
    
    //#[test]
    fn test_g_permutation() {
        let mut key = TEST_KEY;
        let r = whitening(&TEST_KEY_COMPRESSED, &TEST_BLOCK);
        // test encryption
        //assert_eq!(g_permutation(r[0], 0, &mut key, true), 0xf889);
        //assert_eq!(g_permutation(r[1], 0, &mut key, true), 0x7781);

        let mut key = TEST_KEY;
        let r = whitening(&TEST_KEY_COMPRESSED, &TEST_BLOCK);
        //assert_eq!(g_permutation(r[0], 0, &mut key, false), 0xf889);
        //assert_eq!(g_permutation(r[1], 0, &mut key, false), 0x7781);
    }

    #[test]
    fn test_f_encrypt() {
        let mut key = TEST_KEY;
        let r = whitening(&TEST_KEY_COMPRESSED, &TEST_BLOCK);
        assert_eq!(f_encrypt(r[0], r[1], 0, &mut key, true), (0x3eb1, 0xa4e9));

        let mut key = TEST_KEY;
        let r = whitening(&TEST_KEY_COMPRESSED, &TEST_BLOCK_E);
        assert_eq!(f_encrypt(r[0], r[1], 0, &mut key, false), (0xbdbc, 0xbd70));
    }

    #[test]
    fn test_key_schedule() {
        let mut key = TEST_KEY;
        let r = whitening(&TEST_KEY_COMPRESSED, &TEST_BLOCK);
        let g1: u8 = (r[0] >> 8) as u8;
        let g2: u8 = r[0] as u8;
        let g3: u8 = F_TABLE[(g2 ^ key_schedule(&0, &mut key, true)) as usize] ^ g1;
        let g4: u8 = F_TABLE[(g3 ^ key_schedule(&(4*0+1), &mut key, true)) as usize] ^ g2;
        let g5: u8 = F_TABLE[(g4 ^ key_schedule(&(4*0+2), &mut key, true)) as usize] ^ g3;
        let g6: u8 = F_TABLE[(g5 ^ key_schedule(&(4*0+3), &mut key, true)) as usize] ^ g4;
        assert_eq!(g1, 0xaa);
        assert_eq!(g2, 0xee);
        assert_eq!(g3, 0xf3);
        assert_eq!(g4, 0x76);
        assert_eq!(g5, 0xf8);
        assert_eq!(g6, 0x89);

        // test decrypt
        let mut key = TEST_KEY;
        let r = whitening(&TEST_KEY_COMPRESSED, &TEST_BLOCK_E);
        let mut subkeys: Vec<u8> = Vec::<u8>::with_capacity(12);
        for i in (0..12) {
            subkeys.push(key_schedule(&(4*0+(i%4)), &mut key, false));
        }

        //subkeys.push(key_schedule(&(4*0+2), &mut key, false));
        //subkeys.push(key_schedule(&(4*0+3), &mut key, false));

        let g1: u8 = (r[0] >> 8) as u8;
        let g2: u8 = r[0] as u8;
        let g3: u8 = F_TABLE[(g2 ^ subkeys[11]) as usize] ^ g1;
        let g4: u8 = F_TABLE[(g3 ^ subkeys[10]) as usize] ^ g2;
        let g5: u8 = F_TABLE[(g4 ^ subkeys[9]) as usize] ^ g3;
        let g6: u8 = F_TABLE[(g5 ^ subkeys[8]) as usize] ^ g4;
        assert_eq!(g1, 0x31);
        assert_eq!(g2, 0xbb);
        assert_eq!(g3, 0xc7);
        assert_eq!(g4, 0x49);
        assert_eq!(g5, 0x9a);
        assert_eq!(g6, 0xc3);
    }
}