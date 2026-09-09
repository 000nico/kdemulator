use crate::core::cpu_engine::CpuEngine;
use unicorn_engine::Unicorn;
use unicorn_engine::unicorn_const::{Arch, Mode};

pub struct UnicornWrapper {
    uc: Unicorn<'static, ()>,
}

impl CpuEngine for UnicornWrapper {
    fn new() -> Result<Self, String> {
        let uc = Unicorn::new(Arch::X86, Mode::MODE_64).map_err(|e| format!("{:?}", e))?;

        // stack
        uc.mem_map(0x1000, 1024 * 1024, Permission::READ | Permission::WRITE)?;
        self.set_register(Register::RSP, 0x1000 + 1024 * 1024);
    
        Ok(Self { uc })
    }

    fn get_register(&self, reg: Register) -> u64 {
        match reg {
            RSP => self.uc.reg_read(Register86::RSP),
            RIP => self.uc.reg_read(Register86::RIP),
            RFLAGS => self.uc.reg_read(RegisterX86::RFLAGS),
        }
    }

    fn set_register(&mut self, reg: Register, value: u64) {
        match reg {
            RSP => self.uc.reg_write(RegisterX86::RSP, value),
            RIP => self.uc.reg_write(RegisterX86::RIP, value),
            RFLAGS => self.uc.reg_write(RegisterX86::RFLAGS, value),
        }
    }
}
