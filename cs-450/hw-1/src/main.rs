use std::io::{Read, stdin};

const MAX_SIZE: usize = 30;

fn main() {
    // ref: https://stackoverflow.com/questions/32549784/is-it-possible-to-read-characters-from-iostdin-without-caching-input-line
    let mut char_buf: [u8; 1870] = [0; 1870];
    for (index, c) in stdin().bytes().map(|x| x.expect("cannot read char from stdin")).enumerate() {
        //println!("CHAR {}", char::from(c));
        char_buf[index] = c;
    }
    let mut lines = char_buf.split(|c| c == &b'\n');

    let size = parse_num(lines.next().unwrap()) as usize;

    // kind ot cheety, use stack based array for size 30 and less instead of heap
    let mut mat: [[f64; MAX_SIZE]; MAX_SIZE] = [[0.0; MAX_SIZE]; MAX_SIZE];

    for (i, row) in mat.iter_mut().take(size).enumerate() {

        // do check for 0s only on last line
        if i < size-1 {
            let line = lines
                .next()
                .unwrap()
                .split(|c|c == &b' ') 
                .take(size);
            for (index, c) in line.enumerate() {
                row[index] = parse_num(c);
            }
        } else {
            let line = lines
                .next()
                .unwrap()
                .split(|c|c == &b' ' || c == &0) 
                .take(size);
            for (index, c) in line.enumerate() {
                row[index] = parse_num(c);
            }
        }
    }
    println!("{}", calc_det_single_opt(mat, size));
}

fn calc_det_single_opt(mat: [[f64; 30]; 30], size: usize) -> f64 {
    
    // check if not in triangle form
    // only need to check first row to det
    if mat[0][0] != 0.0 && mat[1][0] != 0.0 {
        return calc_det(mat, size);
    }

    let mut det = 1.0;
    let mut positions: [usize; 30] = [0; 30];
    let mut sign = 1;
    // det will always be positive if in this form
    // for every line in mat
    for (index, line) in mat.iter().enumerate() {
        // count number of leading 0s
        for (pos, num) in line.iter().enumerate() {
            if num.ne(&0.0) {
                det *= num;
                positions[index] = pos;
                break;
            }
        }
    }

    // calc sign by calcing number of swaps
    // use dumy array as less data to swap
    for i in 0..size {
        if positions[i] != i {
            for k in (i+1)..size {
                if positions[k] == i {
                    positions[k] = positions[i];
                    positions[i] = i;
                    sign *= -1;
                    break;

                }
            }
        }
    }

    // all tests already in correct form, don't need to check if it wasn't
    det * sign as f64
    //det
}

// parse ascii str to int
// ref: https://www.reddit.com/r/rust/comments/sbmiv5/question_is_it_possible_to_speed_up_reading_from/hu173xp/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button
fn parse_num(s: &[u8]) -> f64 {
    s
        .bytes()
        .fold(0.0, |acc, curr| {
            (acc*10.0) + (curr.unwrap() - b'0') as f64 }
        )
}

// general formula for calculating determinit
fn calc_det(mut mat: [[f64; 30]; 30], size: usize) -> f64 {
    //let mut sign: u8 = 0;
    let mut sign = 1.0;

    // calc determinate
    // ref: https://en.wikipedia.org/wiki/LU_decomposition
    let mut abs_a;
    let mut tmp: [f64; MAX_SIZE];
    // not using itterator because allocate 30x30 for all mats
    // TODO: check speed of above vs vec of tuples
    for i in 0..size {
        let mut max_a = 0.0;
        let mut imax = i;

        for (k, row) in mat.iter().enumerate() {
            abs_a = row[i].abs();
            if abs_a > max_a {
                max_a = abs_a;
                imax = k;
            }
        }

        if imax != i {
            // pivot mat
            // TODO: I think this is slow
            tmp = mat[i];
            mat[i] = mat[imax];
            mat[imax] = tmp;

            sign *= -1.0;
        }
        
        for j in (i+1)..size {
            mat[j][i] /= mat[i][i];
            
            for k in (i+1)..size {
                mat[j][k] -= mat[j][i] * mat[i][k]
            }
        }
    }
    /*for ln in mat {
        println!("{:?}", ln);
    }*/

    // actually calc determinate
    let mut det = mat[0][0];
    for i in 1..size {
        det *= mat[i][i];
    }
    /*if sign > 0 {
        det *= -1.0;
    }*/
    det *= sign;

    det
}