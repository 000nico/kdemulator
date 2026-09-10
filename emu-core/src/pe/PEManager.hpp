class PEManager {
    public:
        bool load_pe();

    private:
        bool read_pe_from_disk();
        bool resolve_iat();
};
