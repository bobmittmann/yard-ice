library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.math_real.all;

entity jtag_desc_ram is
generic 
(
 	DATA_WIDTH : natural := 16;
	ADDR_WIDTH : natural := 8;
	DESC_DATA_BITS : natural := 32;
	DESC_ADDR_BITS : natural := 7
);
port 
(
	clk : in std_logic;

	desc_addr : in std_logic_vector(DESC_ADDR_BITS - 1 downto 0);
	io_addr : in std_logic_vector(ADDR_WIDTH - 1 downto 0);
	io_we : in std_logic;
	io_data : in std_logic_vector(DATA_WIDTH - 1 downto 0);
	desc_q : out std_logic_vector(DESC_DATA_BITS - 1 downto 0);
	io_q : out std_logic_vector(DATA_WIDTH - 1 downto 0)
);
end jtag_desc_ram;

architecture rtl of jtag_desc_ram is
	-- Build a 2-D array type for the RAM
	subtype word_t is std_logic_vector((DATA_WIDTH - 1) downto 0);
	type memory_t is array(2**ADDR_WIDTH - 1 downto 0) of word_t;
	
	-- Declare the RAM signal.	
	signal ram_lo : memory_t;
	signal ram_hi : memory_t;
--	shared variable ram_lo : memory_t;
--	shared variable ram_hi : memory_t;

	signal s_desc_addr : natural range 0 to 2**DESC_ADDR_BITS - 1;
	signal s_io_addr : natural range 0 to 2**DESC_ADDR_BITS - 1;
	
	signal s_we_lo : std_logic;
	signal s_we_hi : std_logic;
	
	signal s_desc_lo : word_t;
	signal s_desc_hi : word_t;

--	signal s_io_dout_lo : word_t;
--	signal s_io_dout_hi : word_t;
begin
	s_io_addr <= to_integer(unsigned(io_addr(ADDR_WIDTH - 1 downto 1)));
	s_we_lo <= io_we and not io_addr(0);
	s_we_hi <= io_we and io_addr(0);

	s_desc_addr <= to_integer(unsigned(desc_addr));

	process(clk)
	begin
		if (rising_edge(clk)) then
			if (s_we_lo = '1') then
				ram_lo(s_io_addr) <= io_data;
--				ram_lo(s_io_addr) := io_data;
			end if;
			s_desc_lo <= ram_lo(s_desc_addr); 
		end if;
	end process;

	process(clk)
	begin
		if (rising_edge(clk)) then
			if (s_we_hi = '1') then
				ram_hi(s_io_addr) <= io_data;
--				ram_hi(s_io_addr) := io_data;
			end if;
			s_desc_hi <= ram_hi(s_desc_addr); 			
		end if;
	end process;

	desc_q(DATA_WIDTH - 1 downto 0) <= s_desc_lo;
	desc_q((DATA_WIDTH * 2) - 1 downto DATA_WIDTH) <= s_desc_hi;

--	process(clk)
--	begin
--		if (rising_edge(clk)) then
--			s_io_dout_hi <= ram_hi(s_io_addr); 
--		end if;
--	end process;

--	process(clk)
--	begin
--		if (rising_edge(clk)) then
--			s_io_dout_lo <= ram_lo(s_io_addr); 
--		end if;
--	end process;

	io_q <= (others => '0');
--	io_q <= s_io_dout_hi when io_addr(0) = '1' else s_io_dout_lo;

end rtl;
