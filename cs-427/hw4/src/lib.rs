

pub fn exp(args: &[String]) -> u128 {
    let mut a: u128 = args[3].parse().unwrap();
    let mut b: u128 = args[5].parse().unwrap();
    let n: u128 = args[7].parse().unwrap();

    let mut total = 0;

    while b > 0 {
        if (b & 0b1) == 0b1 {
            total *= a;
        }
        a *= a;
        b >>= 1;
    }

    total % n
}

pub fn miller_rabin(args: &Vec<String>) -> u128 {
    return 0;
}