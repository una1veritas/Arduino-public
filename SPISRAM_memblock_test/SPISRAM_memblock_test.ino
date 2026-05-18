#include <SPI.h>
#include <SPISRAM.h>


const int SRAM_CS = 10;

// Variable memory block array
struct MemoryPageArray {
	SPISRAM auxarray;
	uint32_t head_ix; // indexes of the first page block.
	uint32_t pages_count;

	struct Page64 {
		static const uint16_t page_size = 64;

		uint32_t address;	// absolute address where data[0] must be aligned and be placed.
		uint16_t length; 		//
		uint8_t  data[page_size];

		Page64(uint32_t & addr, uint8_t data[], uint16_t len) :
			address( addr ),
			length( len ) {
			uint32_t page_boundary = (address & ~uint32_t(page_size - 1)) + page_size;
			if ( address + len > page_boundary) {
				length = page_boundary - address;
			}
			for(uint16_t i = 0; i < length; ++i) {
				this->data[i] = data[i];
			}
		}

		Page64(void) {}

		uint8_t & operator[](const uint16_t & index) {
			return data[index];
		}

		bool is_full() const {
			uint32_t page_boundary = (address & ~uint32_t(page_size - 1)) + page_size;
			return address + length == page_boundary;
		}

		bool is_aligned() const {
			return (address & (page_size - 1)) == 0 ;
		}

		uint16_t remaining_capacity() {
			uint32_t page_boundary = (address & ~uint32_t(page_size - 1)) + page_size;
			return page_boundary - (address + length);
		}

		size_t printOn(Stream & out) {
			char tmp16[16];
			size_t count = 0;
			for(int i = 0; i < length; ++i) {
				if ( i % 16 == 0 ) {
					count += Serial.println();
					snprintf(tmp16, 15, "%04X : ", address + i);
					count += out.print(tmp16);
				}
				snprintf(tmp16, 15, "%02X ", data[i]);
				count += Serial.print(tmp16);
			}
			count += out.println();
			return count;
		}

	};

	MemoryPageArray() :
		auxarray(SPISRAM(SRAM_CS, SPISRAM::BUS_WIDTH_23LC1024)),
		head_ix(0),
		pages_count(0) {
	}

	void begin() {
		auxarray.begin();
	}

	inline uint32_t size() { return pages_count; }

	void clear() {
		head_ix = 0;
		pages_count = 0;
	}

	Page64 & load_page(const uint32_t & auxindex, Page64 & page) {
		for(uint32_t i = 0; i < sizeof(Page64); ++i) {
			*(((uint8_t *)&page) + i) = auxarray.read(auxindex + i);
		}
		return page;
	}

	Page64 & store_page(const uint32_t & auxindex, Page64 & page) {
		for(uint32_t i = 0; i < sizeof(Page64); ++i) {
			auxarray.write(auxindex + i, *(((uint8_t *)&page) + i));
		}
		return page;
	}

	// create new page block then add to the next of the last, as tail page block.
	uint16_t append_page(const uint32_t & addr, const uint8_t data[], const uint16_t & length) {
		Page64 pagecache;

		pagecache.address = addr;
		uint32_t page_boundary = (addr & (~uint32_t(pagecache.page_size - 1))) + pagecache.page_size;

		pagecache.length = length > page_boundary - addr ? page_boundary - addr : length ;
		for(uint16_t i = 0; i < pagecache.length ; ++i) {
			pagecache[i] = data[i];
		}
		store_page(pages_count * sizeof(Page64), pagecache);
		pages_count += 1;
		return pagecache.length;
	}
	
	void append_bytes(uint32_t address, uint8_t * data, uint16_t length) {
		Page64 lastpage;
		load_page(head_ix + sizeof(Page64) * (pages_count - 1), lastpage);

		uint16_t ix = 0; // offset index to data
		// update to fill up the last page
		if ( ! lastpage.is_full() and address == lastpage.address + lastpage.length ) {
			uint16_t remain = lastpage.remaining_capacity();
			for( ; ix < remain and ix < length; ++ix) {
				lastpage[lastpage.length + ix] = data[ix];
			}
			lastpage.length += ix;
			store_page((pages_count - 1) * sizeof(Page64), lastpage); 	// now the last page block is full (to the page boundary)
		}
		// append new pages
		for( ; ix < length; ix += lastpage.page_size) {
			uint16_t bytes_stored = append_page(address + ix, data + ix, length - ix);
		}

	}

	size_t printOn(Stream & out) {
		Page64 page;
		size_t count = 0;
		for(uint16_t i = 0; i < pages_count; ++i) {
			load_page(i * sizeof(Page64), page);
			count += page.printOn(Serial);
			count += Serial.println();
		}
		return count;
	}
};

MemoryPageArray memblkmgr;

char buf256[256];

void setup() {
  
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println();
    
  // All SPI devices must be inactivated
  // by pulling CSs high.

  // inactivate other SPI slaves
  digitalWrite(8, HIGH);
  pinMode(8, OUTPUT);
  digitalWrite(9, HIGH);
  pinMode(9, OUTPUT);
  
  SPI.begin();
  memblkmgr.begin();

  for (int i =0; i < 256; i++)
    buf256[i] = i<<1;

Serial.println("MemoryPageArray test:");

  memblkmgr.append_bytes(0, buf256, 165);
//  memblkmgr.print();
  memblkmgr.append_bytes(165, buf256, 32);
  
   for (int i =0; i < 128; i++)
     buf256[i] = (i<<1) + 1;
   memblkmgr.append_bytes(197, buf256, 80);
   memblkmgr.printOn(Serial);
}

void loop()
{
}

