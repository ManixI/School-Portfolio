// global camera object
var the_camera = new Camera;

class Keys{
    constructor() {
        this.keys_down = {}
    }

    static start_listening() {
        let keys = new Keys();

        addEventListener('keydown',function(ev) {
            if(typeof ev.code === 'string') {
                keys.keys_down[ev.code]=true;
            }
        })

        addEventListener('keyup',function(ev) {
            if(typeof ev.code === 'string') {
                keys.keys_down[ev.code] = false;
            }
        })
        return keys;
        }

    keys_down_list() {
        return Object.entries(this.keys_down)
            .filter(kv=>kv[1])
            .map(kv=>kv[0])
    }
}

// camera key bindings
let bindings = {
    'KeyW': function() {move_camera_forward();},
    'KeyS': function() {move_camera_backward();},
    'KeyA': function() {strafe_camera_left();},
    'KeyD': function() {strafe_camera_right();},
    'Space': function() {strafe_camera_up();},
    'KeyC': function() {strafe_camera_down();},
    'KeyE': function() {roll_camera_right();},
    'KeyQ': function() {roll_camera_left();},
    'ArrowRight': function() {rotate_camera_right();},
    'ArrowLeft': function() {rotate_camera_left();},
    'ArrowUp': function() {pitch_camera_down();},
    'ArrowDown': function() {pitch_camera_up();}
}



// bunch of functions to translate the camera object position
function move_camera_forward() {
    the_camera.thing.position.z += -0.01;
    //console.log('KeyW');
}
function move_camera_backward() {
    the_camera.thing.position.z += 0.01;
}
function strafe_camera_left() {
    the_camera.thing.position.x += -0.01;
}
function strafe_camera_right() {
    the_camera.thing.position.x += 0.01;
}
function strafe_camera_up() {
    the_camera.thing.position.y += 0.01;
}
function strafe_camera_down() {
    the_camera.thing.position.y += -0.01
}
function roll_camera_right () {
    the_camera.thing.roll += 0.01;
}
function roll_camera_left () {
    the_camera.thing.roll += -0.01;
}
function rotate_camera_right() {
    the_camera.thing.yaw += 0.005;
}
function rotate_camera_left() {
    the_camera.thing.yaw += -0.005;
}
function pitch_camera_down() {
    the_camera.thing.pitch += -0.005;
}
function pitch_camera_up() {
    the_camera.thing.pitch += 0.005;
}