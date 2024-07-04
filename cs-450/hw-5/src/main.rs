use std::env;

use hw_5::*;

fn main() {
    env::set_var("RUST_BACKTRACE", "1");
    let mut world = read_input();
    //println!("World: {:?}", world);
    /*for line in world.iter() {
        for chr in line {
            print!("{}",*chr as char);
        }
        println!("");
    }*/
    let (mut path, cost) = astr(&world);
    path.reverse();
    print_path(&mut world, &path);
    println!("Total cost: {}", cost);
    //println!("Path: {:?}", path);
}
