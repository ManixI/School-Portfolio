use std::{io::{stdin, Read}, cmp::Ordering, collections::{BinaryHeap, HashMap, HashSet}, ptr, rc::Rc};

pub fn read_input() -> Vec<Vec<u8>> {
    let mut input = stdin().lock();
    let mut input_buf = Vec::<u8>::new();
    let _ = input.read_to_end(&mut input_buf);

    let input_string: String;
    unsafe { input_string = String::from_utf8_unchecked(input_buf);}

    let mut world = Vec::<Vec<u8>>::new();

    for line in input_string.lines() {
        let mut new_line = Vec::<u8>::new();
        for val in line.split_ascii_whitespace() {
            new_line.push(val.as_bytes()[0])
        }
        world.push(new_line);
    }
    world
}


fn find_val(world: &[Vec<u8>], target: u8) -> Position {
    for (row, line) in world.iter().enumerate() {
        for (col, val) in line.iter().enumerate() {
            if val == &target {
                return Position {
                    x: col,
                    y: row,
                };
            }
        }
    }
    Position { x: 0, y: 0 }
}

fn get_square_cost(char: &u8) -> NodeCost {
    match char {
        b'.' => NodeCost::Traversable(1.0),
        b',' => NodeCost::Traversable(2.0),
        b'o' => NodeCost::Traversable(3.0),
        b'=' => NodeCost::Traversable(50.0),
        b'2' => NodeCost::Target,         // target position (impassible)
        b'0' => NodeCost::Impassable,     // starting position
        b'1' => NodeCost::Impassable,     // friendly char (impassible)
        b'3' => NodeCost::Impassable,     // non-target hostile char (impassable)
        b'\n' => NodeCost::Impassable,
        b'\r' => NodeCost::Impassable,
        _ => panic!("Encountered bad char in map: {}", char),
    }
}
//----------------------------------------------------------

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct Position {
    x: usize,
    y: usize,
} impl Position {
    fn new(x: usize, y: usize) -> Self {
        Position { x, y }
    }

    fn dist(&self, other: &Self) -> f32 {
        let x_1 = self.x as f32;
        let x_2 = other.x as f32;
        let y_1 = self.y as f32;
        let y_2 = other.y as f32;

        // TODO: speed up by removing sqrt and possibly pow func calls
        ((x_1 + x_2).powf(2.0) + (y_1 + y_2).powf(2.0)).sqrt() * 1.4 // found though guess and check 
    }
}

#[derive(Debug, Clone, Copy)]
enum NodeCost {
    Traversable(f32),
    Target,
    Impassable,
} impl PartialEq for NodeCost {
    fn eq(&self, other: &Self) -> bool {
        matches!(self.cmp(other), Ordering::Equal)
    }
} impl Eq for NodeCost {    
} impl Ord for NodeCost {
    fn cmp(&self, other: &Self) -> Ordering {
        match (self, other) {
            (NodeCost::Target, _) => Ordering::Less,
            (NodeCost::Impassable, _) => Ordering::Greater,
            (NodeCost::Traversable(_), NodeCost::Impassable) => Ordering::Less,
            (NodeCost::Traversable(_), NodeCost::Target) => Ordering::Greater,
            (NodeCost::Traversable(a), NodeCost::Traversable(b)) => a.partial_cmp(b).unwrap(),
        }
    }
} impl PartialOrd for NodeCost {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
} impl NodeCost {
    // the things I do to mutate in place
    fn mul(&mut self, val: f32) {
        unsafe {
            ptr::write(self, match ptr::read(self) {
                Self::Traversable(v) => Self::Traversable(v * val),
                v => v,
            })
        }
    }

    fn add(&mut self, other: &Self) {
        if let Self::Traversable(val) = other {
            unsafe {
                ptr::write(self, match ptr::read(self) {
                    Self::Traversable(v) => Self::Traversable(v + val),
                    v => v,
                })
            }
        }
    }

    fn get_inner(&self) -> Option<f32> {
        match self {
            Self::Traversable(v) => Some(*v),
            _ => None,
        }
    }
}

#[derive(Debug, Clone)]
struct Edge {
    end_position: Position,
    total_cost: NodeCost,
    distance: f32,     // truncated
    start_position: Option<Rc<Edge>>,
} impl Ord for Edge {
    fn cmp(&self, other: &Self) -> Ordering {
        match (other.total_cost, self.total_cost) {
            (NodeCost::Impassable, NodeCost::Impassable) => Ordering::Equal,
            (NodeCost::Target, _) => Ordering::Less,
            (_, NodeCost::Target) => Ordering::Greater,
            (NodeCost::Traversable(a), NodeCost::Traversable(b)) => (a + self.distance)
                                                                                .partial_cmp(&(b + other.distance))
                                                                                .unwrap(),
            (NodeCost::Impassable, _) => Ordering::Greater,
            (_, NodeCost::Impassable) => Ordering::Less,
        }
    }
} impl PartialOrd for Edge {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
} impl PartialEq for Edge {
    fn eq(&self, other: &Self) -> bool {
        if self.cmp(other) == Ordering::Equal {
            return true;
        }
        false
    }
} impl Eq for Edge {
}

pub fn print_path(world: &mut [Vec<u8>], path: &Vec<Position>) {
    for node in path {
        world[node.y][node.x] = b'*';
    }

    let mut out = String::new();
    for line in world.iter() {
        for c in line {
            out.push(*c as char);
            out.push(' ');
        }
        out.push('\n');
    }
    print!("{}", out); 
}

// returns a vec of the path plus the cost
pub fn astr(world: &Vec<Vec<u8>>) -> (Vec<Position>, f32) {
    let mut frontier = BinaryHeap::<Edge>::new();
    let mut visited = HashSet::<Option<Position>>::new();
    // map of coords and best cost to reach said coord
    let mut nodes = HashMap::<Position, f32>::new();

    let start = find_val(world, b'0');
    let target = find_val(world, b'2');

    nodes.insert(start, 0.0);
    let mut cur_node = Rc::new(Edge { 
        start_position: None, 
        end_position: start, 
        total_cost: NodeCost::Traversable(0.0), 
        distance: start.dist(&target) 
    });
    for node in get_neibors(world, &cur_node, &target) {
        frontier.push(node);
    }
    let _ = visited.insert(Some(start));

    loop {
        if cur_node.end_position == target {
            break;
        }
        cur_node = Rc::new(frontier.pop().unwrap());
        

        for node in get_neibors(world, &cur_node, &target) {
            if !visited.insert(Some(node.end_position)) {
                // already visited node implicitly for lower cost
                continue;
            }
            frontier.push(node);
        }
    }

    let cost = cur_node.start_position.as_ref().unwrap().total_cost.get_inner().unwrap();
    let mut out_vec = Vec::<Position>::new();
    let mut position = cur_node.start_position.as_ref().unwrap();
    while position.start_position.is_some() {
        out_vec.push(position.end_position);
        position = position.start_position.as_ref().unwrap();
    }

    (out_vec, cost)
}

fn get_neibors(world: &Vec<Vec<u8>>, node: &Rc<Edge>, target: &Position) -> Vec<Edge> {
    let mut out = Vec::<Edge>::with_capacity(8);
    let pos = node.end_position;

    let upper_boud = (world.len() - 1) as isize;
    let outer_bound = (world[0].len() - 1) as isize;

    // there's got to be a better way then all these type conversions
    let y = pos.y as isize;
    let x = pos.x as isize;

    for y in y-1..y+2 {
        if y < 0 || y > upper_boud {
            continue;
        }
        for x in x-1..x+2 {
            if x < 0 || x > outer_bound {
                continue;
            }
            let mut edge = Edge {
                start_position: Some(Rc::clone(node)),
                end_position: Position::new(x as usize, y as usize),
                distance: pos.dist(target),
                total_cost: get_square_cost(&world[y as usize][x as usize])
            };
            // if a corner node
            if x != pos.x as isize && y != pos.y as isize {
                edge.total_cost.mul(1.5);
            }
            edge.total_cost.add(&node.total_cost);
            out.push(edge);
        }
    }
    out
}