use std::env;
use std::time::{Duration, SystemTime};
use hw_2::*;

// ./target/release/hw-2 default < src/players_sample.dat > output.txt
// diff output.txt src/players_sample_out.txt

const PRINT: bool = true;

fn main() {
    let args: Vec<String> = env::args().collect();

    let mut start: Duration = Default::default();
    let mut time_read: Duration = Default::default();
    let mut time_sum: Duration = Default::default();
    let mut time_sort: Duration = Default::default();
    if PRINT {
        start = SystemTime::now().duration_since(SystemTime::UNIX_EPOCH).unwrap();
    }
    //let mut arr = read_file_stdin_opt();
    let mut arr = read_file_stdin_nieve();
    //println!("1");
    if PRINT {
        time_read = SystemTime::now().duration_since(SystemTime::UNIX_EPOCH).unwrap();
    }
    create_sum_vec(&mut arr);
    //println!("2");
    if PRINT {
        time_sum = SystemTime::now().duration_since(SystemTime::UNIX_EPOCH).unwrap();
    }
    match args[1].as_str() {
        "custom" => arr = count_sort_partle(arr),
        "threaded" => arr = count_sort_partle_threaded(arr),
        "default" => default_sort(&mut arr),
        _ => panic!("bad argument")
    }
    //println!("3");
    if PRINT {
        time_sort = SystemTime::now().duration_since(SystemTime::UNIX_EPOCH).unwrap();
    }
    print_arr(arr);
    //println!("4");
    if PRINT {
        let time_print = SystemTime::now().duration_since(SystemTime::UNIX_EPOCH).unwrap();
        println!("Time to read: {:?}", time_read-start);
        println!("Time to sum: {:?}", time_sum-time_read);
        println!("Time to sort: {:?}", time_sort-time_sum);
        println!("Time to print: {:?}", time_print-time_sort);
        println!("Totak time: {:?}", time_print - start);
    }
    
}
