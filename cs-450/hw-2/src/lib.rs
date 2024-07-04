#![feature(portable_simd)]
#![feature(test)]
#![feature(slice_swap_unchecked)]


use std::io::{BufReader, BufRead, Read, stdin};
use std::fs::File;
use std::{simd::*, vec};
use std::thread;

const ATTRIBUTES: [&str; 6] = [
    "SKILL_BREAKDANCING",
    "SKILL_APICULTURE",
    "SKILL_BASKET",
    "SKILL_XBASKET",
    "SKILL_SWORD",
    "TOTAL_XP",
];

const LOW_BOUNDS: usize = 0;
const HIGH_BOUNDS: usize = 2000;
const LOW_BOUNDS_SUM: usize = LOW_BOUNDS * 4;
const HIGH_BOUNDS_SUM: usize = HIGH_BOUNDS * 5;

pub fn read_file(filename: &str) -> [Vec<u32>; 6] {
    let mut arr: [Vec<u32>; 6] = [(); 6].map(|_| Vec::with_capacity(100));

    let file = File::open(filename).unwrap();
    let mut reader = BufReader::new(file);

    let mut char_buf: Vec<u8> = Vec::with_capacity(100);
    // reading from EOF will return 0, not Err
    // TODO: test if read_until is faster then read_line
    let mut var;
    while reader.read_until(b'\n', &mut char_buf).unwrap() != 0 {
        for (i, val) in char_buf.split(|c| c == &b' ').enumerate() {
            if i == 4 {
                var = parse_num(&val[0..val.len()-1]);
                // drop LF if last val in line
            } else {
                var = parse_num(val);
            }
            arr[i].push(var);
        }
        char_buf.clear();
    } 
    create_sum_vec(&mut arr);

    arr
}

pub fn read_file_opt(filename: &str) -> [Vec<u32>; 6] {
    let mut arr: [Vec<u32>; 6] = [(); 6].map(|_| Vec::with_capacity(100));

    let file = File::open(filename).unwrap();
    let mut reader = BufReader::new(file);

    let mut char_buf: Vec<u8> = Vec::with_capacity(100);
    // reading from EOF will return 0, not Err
    // TODO: test if read_until is faster then read_line
    let mut var;
    while reader.read_until(b'\n', &mut char_buf).unwrap() != 0 {
        for (i, val) in char_buf.split(|c| c == &b' ').enumerate() {
            if i == 4 {
                var = parse_num(&val[0..val.len()-1]);
                // drop LF if last val in line
            } else {
                var = parse_num(val);
            }
            arr[i].push(var);
        }
        char_buf.clear();
    } 
    create_sum_vec(&mut arr);
    //create_sum_vec_simd(&mut arr);

    arr
}

pub fn create_sum_vec(arr: &mut [Vec<u32>; 6]) {
    let mut sum = vec![0; arr[0].len()];
    for row in arr.iter() {
        for (x, y) in sum.iter_mut().zip(row.iter()) {
            *x += *y;
        }
    }
    arr[5] = sum;
}

// ref: https://www.cs.brandeis.edu/~cs146a/rust/rustbyexample-02-21-2015/simd.html
// not actually faster, not sure if too small or if nieve approach is optomised to be equivelent                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
pub fn create_sum_vec_simd(arr: &mut [Vec<u32>; 6]) {
    let mut sum = vec![0; arr[0].len()];

    let size = arr[0].len() as isize;
    let chunks = size / 64;

    for v in arr.iter().take(5) {
        let tmp = v;
        let p_x: *mut u32 = sum.as_mut_ptr();
        let p_y: *const u32 = tmp.as_ptr();

        for i in (64 * chunks)..size {
            unsafe {
                *p_x.offset(i) += *p_y.offset(i);
            }
        }

        let simd_p_x = p_x as *mut u32x64;
        let simd_p_y = p_y as *const u32x64;

        for i in 0..chunks {
            unsafe {
                // this works, but only if you ignore a debug panic
                *simd_p_x.offset(i) += *simd_p_y.offset(i);
            }
        }
    }
    arr[5] = sum;
}

pub fn read_file_stdin_nieve() -> [Vec<u32>; 6] {
    let mut arr: [Vec<u32>; 6] = [(); 6].map(|_| Vec::with_capacity(100));
    let mut input = stdin().lock();
    let mut buffer = String::new();

    let _ = input.read_to_string(&mut buffer);

    for line in buffer.lines() {
        for (i, val) in line.split_ascii_whitespace().enumerate() {
            let val = val.parse().unwrap();
            arr[i].push(val);
        }
    }
    arr
}

/**
 * multi-thread
 * create threadpool with 8 threads
 * read one thread worth of stdin in then send to start parsing, then summing
 * then use threads to sort
 * TODO: this is both broken and slow
 */
pub fn read_file_stdin() -> [Vec<u32>; 6] {
    // TODO: bug reading 0s
    let mut arr: [Vec<u32>; 6] = [(); 6].map(|_| Vec::with_capacity(100));
    let mut input = stdin().lock();

    let mut buf: Vec<u8> = Vec::new();
    let mut line_buf: Vec<Vec<u8>> = Vec::new();
    while input.read_until(b'\n', &mut buf).unwrap() !=0 {
        // TODO: use memcpy/don't use clone
        line_buf.push(buf.to_vec());
        buf.clear();
    }

    //let mut lines = char_buf.split(|c| c == &b'\n');

    for line in line_buf {
        for (col, val) in line.split(|c| c==&b' ').enumerate() {
            arr[col].push(parse_num(val));
        }
    }

    arr
}

// same as above but read all at once then parse.
pub fn read_file_stdin_opt() -> [Vec<u32>; 6] {
    // TODO: bug reading 0s
    let mut arr: [Vec<u32>; 6] = [(); 6].map(|_| Vec::with_capacity(100));
    let mut input = stdin().lock();

    let mut buf: Vec<u8> = Vec::new();
    let _ = input.read_to_end(&mut buf);
    for line in buf.split(|c| c==&b'\n') {
        for (col, val) in line.split(|c| c==&b' ').enumerate() {
            arr[col].push(parse_num(val));
        }
    }
    // bug with \0 being interperted as extra 0 in first col
    let _ = arr[0].pop();
    arr
}

// ref: https://www.reddit.com/r/rust/comments/sbmiv5/question_is_it_possible_to_speed_up_reading_from/hu173xp/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button
fn parse_num(s: &[u8]) -> u32 {
    // TODO: I can speed this up with simd?
    s.bytes().fold(0, |acc, cur| (acc*10) + (cur.unwrap() & 0x0f) as u32)
}

pub fn default_sort(arr: &mut [Vec<u32>; 6]){
    for attrib in arr {
        // sort large to small, sort_unstable() sorts small to large
        attrib.sort_unstable_by(|a, b| b.cmp(a));
    }
}

pub fn count_sort_partle_threaded(arr: [Vec<u32>; 6]) -> [Vec<u32>; 6] {
    let mut sorted: [Vec<u32>; 6] = [(); 6].map(|_| Vec::with_capacity(100));

    let mut threads = Vec::with_capacity(6);
    let mut arr = arr.into_iter();
    for _ in 0..5 {
        let ln = arr.next().unwrap();
        threads.push(thread::spawn(move || {
            let mut outlires: Vec<u32> = Vec::new();
            
            let mut count_arr = vec![0; HIGH_BOUNDS - LOW_BOUNDS];

            for val in ln.iter() {
                let var = *val as usize;
                if LOW_BOUNDS < var && var < HIGH_BOUNDS {
                    count_arr[var-LOW_BOUNDS] += 1;
                } else {
                    outlires.push(*val);
                }
            }
            quick_sort_n(&mut outlires);
            let count_arr = expand(count_arr, LOW_BOUNDS as u32);

            merge(count_arr, &mut outlires)
        }));
    }

    let ln = arr.next().unwrap();
        threads.push(thread::spawn(move || {
            let mut outlires: Vec<u32> = Vec::new();
            
            let mut count_arr = vec![0; HIGH_BOUNDS_SUM - LOW_BOUNDS_SUM];

            for val in ln.iter() {
                let var = *val as usize;
                if LOW_BOUNDS_SUM < var && var < HIGH_BOUNDS_SUM {
                    count_arr[var-LOW_BOUNDS_SUM] += 1;
                } else {
                    outlires.push(*val);
                }
            }
            quick_sort_n(&mut outlires);
            let count_arr = expand(count_arr, LOW_BOUNDS_SUM as u32);

            merge(count_arr, &mut outlires)
        }));

    for (i, thread) in threads.into_iter().enumerate() {
        sorted[i] = thread.join().unwrap();
    }

    sorted
}

pub fn count_sort_partle(mut arr: [Vec<u32>; 6]) -> [Vec<u32>; 6] {

    let mut outlires: Vec<u32> = Vec::new();

    let cols = arr.clone().into_iter().take(5);
    
    for (i, col) in cols.enumerate() {
        // normal case
        //ref: https://stackoverflow.com/questions/68049221/rust-array-initialization-from-range-or-collect
        let mut count_arr = vec![0; HIGH_BOUNDS - LOW_BOUNDS];

        // TODO: can I do this with simd
        for val in col.iter() {
            let var = *val as usize;
            if LOW_BOUNDS < var && var < HIGH_BOUNDS {
                count_arr[var-LOW_BOUNDS] += 1;
            } else {
                outlires.push(*val);
            }
        }
        quick_sort_n(&mut outlires);
        let count_arr = expand(count_arr, LOW_BOUNDS as u32);
        let sorted = merge(count_arr, &mut outlires);
        arr[i] = sorted;
        outlires.clear();
    }
    
    // different range for sum col
    let mut count_arr = vec![0; HIGH_BOUNDS_SUM-LOW_BOUNDS_SUM];
    
    for val in arr[5].iter() {
        let var = *val as usize;
        if LOW_BOUNDS_SUM < var && var < HIGH_BOUNDS_SUM {
            count_arr[var-LOW_BOUNDS_SUM] += 1;
        } else {
            outlires.push(*val);
        }
    }
    quick_sort_n(&mut outlires);
    let count_arr = expand(count_arr, LOW_BOUNDS_SUM as u32);
    let sorted = merge(count_arr, &mut outlires);
    arr[5] = sorted;

    arr
}

fn expand(arr: Vec<u32>, low: u32) -> Vec<u32> {
    let mut expanded_arr: Vec<u32> = Vec::new();
    for (index, val) in arr.iter().enumerate() {
        for _ in 0..*val {
            expanded_arr.push(index as u32 + low);
        }
    }

    expanded_arr
}


// ref: https://iq.opengenus.org/quick-sort-in-rust/
fn quick_sort_n(slice: &mut [u32]) {
    if !slice.is_empty() {
        let partition_index = partition_n(slice);
        let len = slice.len();

        quick_sort_n(&mut slice[0..partition_index]);
        quick_sort_n(&mut slice[partition_index + 1..len]);
        assert_sorted(slice);
    }
}

fn partition_n(slice: &mut [u32]) -> usize {
    let len = slice.len();
    let pivot = slice[len - 1];
    let mut i = 0;
    let mut j = 0;

    while j < len - 1 {
        if slice[j] <= pivot {
            slice.swap(i, j);
            i += 1;
        }
        j += 1;
    }

    slice.swap(i, len - 1);

    i
}

fn assert_sorted(slice: &[u32]) {
    for i in 1..slice.len() {
        assert!(slice[i - 1] <= slice[i])
    }
}

pub fn merge(mut arr1: Vec<u32>, arr2:  &mut Vec<u32>) -> Vec<u32> {
    let mut sorted: Vec<u32> = Vec::new();

    // TODO: simplifie this, arr2 always all goes before arr1
    let mut left = arr1.pop();
    let mut right = arr2.pop();
    loop {
        //println!("{:?}", (left, right));
        match (left, right) {
            (None, None) => return sorted,
            (None, Some(y)) => {
                sorted.push(y);
                for _ in 0..arr2.len() {
                    sorted.push(arr2.pop().unwrap());
                }
                return sorted;
            },
            (Some(x), None) => {
                sorted.push(x);
                for _ in 0..arr1.len() {
                    sorted.push(arr1.pop().unwrap());
                }
                return sorted;
            }
            (Some(x), Some(y)) => {
                if x > y {
                    sorted.push(x);
                    left = arr1.pop();
                } else {
                    sorted.push(y);
                    right = arr2.pop();
                }
            },
        }
    }

    // this is insainly slow
    /*
    arr1.reverse();
    for val in arr2 {
        arr1.insert(0, *val);
    }

    arr1
    */
}

// ref: https://renato.athaydes.com/posts/how-to-write-fast-rust-code.html
pub fn print_arr(arr: [Vec<u32>; 6]) {

    for (i, col) in arr.iter().enumerate() {
        println!("{}\n{}\n", ATTRIBUTES[i], col.iter()
            .map(|v| v.to_string())
            .collect::<Vec<_>>()
            .join("\n"));
    }
    // not actually faster
    /*let mut out = stdout().lock(); 
    for (i, col) in arr.iter().enumerate() {
        let _ = writeln!(out, "{}", ATTRIBUTES[i]);
        for val in col {
            let _ = writeln!(out, "{}", val);
        }
        let _ = writeln!(out);
    }*/
}

pub fn print_arr_nieve(arr: [Vec<u32>; 6]) {
    for (i, col) in arr.iter().enumerate() {
        println!("{}", ATTRIBUTES[i]);
        for val in col {
            println!("{}", val);
        }
        println!();
    }
}


#[cfg(test)]
mod test {
    use crate::{read_file, default_sort};

    const FILE_PATH: &str = "/home/ian/dev/cs-450/hw-2/src/players_sample.dat";

    #[test]
    fn test_read_file() {
        let arr = read_file(FILE_PATH);
        for e in arr {
            println!("{:?}\n", e);
        }
    }

    #[test]
    fn test_basic_sort() {
        let mut arr = read_file(FILE_PATH);
        default_sort(&mut arr);
        for e in arr {
            println!("{:?}\n", e);
        }
    }
}