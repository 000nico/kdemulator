pub enum Register {
    RSP,
    RIP,
    RFLAGS,
}

pub trait CpuEngine {
    fn new() -> Result<Self, String>
    where
        Self: Sized;

    // registers
    fn get_register(&self, reg: Register) -> u64;
    fn set_register(&mut self, reg: Register, value: u64);
}
