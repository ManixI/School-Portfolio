/**
 * NOTE: Accepted Commands
 * `scapegoat` - Works as expected
 * `btree` - Works as bst
 * `rbt` - Works as bst
 * `avl` - Works as bst
 */

use std::env;
use hw_3::*;

fn main() {
    let args: Vec<String> = env::args().collect();
    env::set_var("RUST_BACKTRACE", "1");
    //let mut arr = read_file_stdin_nieve();
    match args[1].as_str() {
        "avl" => {
            let tree = read_file_btree(args[2].as_str());
            let out = build_out_from_file_btree(&args[3], &tree);
            println!("{}", out);
        },
        "rbt" => {
            let tree = read_file_btree(args[2].as_str());
            let out = build_out_from_file_btree(&args[3], &tree);
            println!("{}", out);
        },
        "btree" => {
            let tree = read_file_btree(args[2].as_str());
            let out = build_out_from_file_btree(&args[3], &tree);
            println!("{}", out);
        },
        "scapegoat" => {
            let tree = read_file_sgt(args[2].as_str());
            let out = build_out_from_file(&args[3], &tree);
            println!("{}", out);
        },
        _ => panic!("bad argument")
    }
}
// .7-.8 alpha is good
