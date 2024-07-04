use std::env;

const BACKTRACE_VAL: &str = "1";

fn main() {
    env::set_var("RUST_BACKTRACE", BACKTRACE_VAL);
    let args: Vec<String> = env::args().collect();

    let mut keyfile: Option<&str> = None;
    let mut infile: Option<&str> = None;
    let mut outfile: Option<&str> = None;

    let mut encrypt: Option<bool> = None;

    for i in 0..args.len() {
        match &args[i][..] {
            "-e" => encrypt = Some(true),
            "-d" => encrypt = Some(false),
            "-in"=> infile = Some(&args[i+1][..]),
            "-out" => outfile = Some(&args[i+1][..]),
            "-k" => keyfile = Some(&args[i+1][..]),
            _ => continue, 
        };
    }

    if encrypt.is_none() {
        panic!("Error: No encryption mode selected");
    }
    if keyfile.is_none() || infile.is_none() || outfile.is_none() {
        panic!("Error: missing args");
    }


}
