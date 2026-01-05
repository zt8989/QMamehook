// [General]
// MameStart=
// MameStop=
// StateChange=
// OnRotate=
// OnPause=
// [Output]
// P1_LmpStart=
// P2_LmpStart=
// P1_LmpPanel=
// P2_LmpPanel=
// P1_GunRecoil=
// P2_GunRecoil=
// P1_GunMotor=
// P2_GunMotor=
// P1_Ammo=
// P2_Ammo=
// P1_Clip=
// P2_Clip=
// P1_CtmRecoil=
// P2_CtmRecoil=
// P1_Life=
// P2_Life=
// P1_Damaged=
// P2_Damaged=

function on_MameStart(context) {
    // Example: open up to two ports if available.
    if(context.portCount >= 1)
        qmh.open(1);
    if(context.portCount >= 2)
        qmh.open(2);
}

function on_MameStop(context) {
    // Example: stop recoil and close ports.
    if(context.portCount >= 1) {
        qmh.write(1, "E");
        qmh.close(1);
    }
    if(context.portCount >= 2) {
        qmh.write(2, "E");
        qmh.close(2);
    }
}

function on_P1_LmpStar(value, context) {
    // Example: drive lamp on/off (value is usually 0/1).
    qmh.write(1, value ? "L1" : "L0");
}

function on_P2_LmpStar(value, context) {
    // Example: second player lamp.
    if(context.portCount >= 2)
        qmh.write(2, value ? "L1" : "L0");
}

function on_P1_GunRecoil(value, context) {
    // Example: recoil on shot (value is usually 0/1).
    qmh.write(1, value ? "R1" : "R0");
    // XInput example: short rumble on player 1 controller.
    qmh.xinputVibrate(0, value ? 50000 : 0, value ? 20000 : 0);
}

function on_P2_GunRecoil(value, context) {
    if(context.portCount >= 2)
        qmh.write(2, value ? "R1" : "R0");
    // XInput example: short rumble on player 2 controller.
    qmh.xinputVibrate(1, value ? 50000 : 0, value ? 20000 : 0);
}


module.exports = {
    on_MameStart,
    on_MameStop,
    on_P1_LmpStar,
    on_P2_LmpStar,
    on_P1_GunRecoil,
    on_P2_GunRecoil
}
