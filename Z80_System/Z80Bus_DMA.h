#ifndef _Z80BUS_DMA_H_
#define _Z80BUS_DMA_H_

struct DMA_Controller {
	static const uint16_t blk_size_base = 128;

	uint16_t address;
	uint16_t blk_size;
	uint8_t page_buffer[256];

	enum DMA_request {
		NO_REQUEST = 0,
		READ_FROM_RAM = 1,  // from ram to page buffer
		WRITE_TO_RAM = 2,  // from page buffer to ram
	} transfer_mode;

	enum DMA_status {
		TRANSFER_OK = 0,
		TRANSFER_FAILED = 0xff,
	} result;

	DMA_Controller() { init(); }

	void init() {
		address = 0;
		blk_size = (blk_size_base << 1);
		transfer_mode = NO_REQUEST;
		result = 0;
	}

	void set_address(uint16_t addr16) {
		address = addr16;
	}
	void set_address_low(uint8_t low8) {
		((uint8_t *)& address)[0] = low8;
	}
	void set_address_high(uint8_t hi8) {
		((uint8_t *)& address)[1] = hi8;
	}

	void set_block_size(uint8_t n) {
		blk_size = blk_size_base << n;
		blk_size = blk_size > 256 ? 256 : blk_size;
	}

	void set_transfer_mode(uint8_t mode){
		transfer_mode = mode;
	}

	uint8_t * buffer() { return page_buffer; }

	uint16_t block_size() const {
		return blk_size;
	}
};

#endif
