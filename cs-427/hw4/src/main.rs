use std::env;
//use hw4::*;

fn main() {
    let args: Vec<String> = env::args().collect();

    let mut val: u128 = 0;

    //
    match &args[1][..] {
        "--exp"=> val = hw4::exp(&args),
        "--mr" => val = hw4::miller_rabin(&args),
        _ => print!("Accepted commands are --exp (exponent) and --mr (Millar-Rabin)")
    };


}
