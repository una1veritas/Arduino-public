#ifndef _Z80BUS_FDC_H_
#define _Z80BUS_FDC_H_

struct DiskType {
	uint16_t nof_tracks;
	uint16_t nof_sectors;
	uint16_t sector_size;

	DiskType() : nof_tracks(77), nof_sectors(26), sector_size(128){}
	DiskType(const uint16_t & ntracks, const uint16_t & nsectors, const uint16_t sectsize) :
		nof_tracks(ntracks), nof_sectors(nsectors), sector_size(sectsize) {}
	DiskType(const DiskType & dt) :
		nof_tracks(dt.nof_tracks), nof_sectors(dt.nof_sectors), sector_size(dt.sector_size) {}
};
static const DiskType IBM8inSS = {77, 26, 128};

struct Disk_Controller {

	enum FDCOP_CODE {
		READ_SECTOR 	= 0,
		WRITE_SECTOR 	= 1,
		NO_REQUEST = 0xff,
	};
  
	enum {
		RES_OK = 0,		/* 0: Successful */
		RES_ERROR,		/* 1: R/W Error */
		RES_WRPRT,		/* 2: Write Protected */
		RES_NOTRDY,		/* 3: Not Ready */
		RES_PARERR		/* 4: Invalid Parameter */
	};

	static const uint8_t nof_drives = 2;

	struct Drive {
		DiskType dtype;
		char dskfname[16];
		File dskfile;
		//
		uint16_t sector;
		uint16_t track;
		uint8_t status;
	} drives[nof_drives];
	uint8_t current_drive;
	uint8_t opcode;

	Disk_Controller() {
		current_drive = 0;
		opcode = NO_REQUEST;
		for(uint8_t i = 0; i < nof_drives; ++i) {
			drive().dskfile.close();
		}
	}

	void init() {}

	void sel_drive(uint8_t dno) {
		current_drive = dno;
	}

	void sel_track(uint8_t tno) {
		drives[current_drive].track = tno;
	}

	void sel_sector(uint8_t sno) {
		drives[current_drive].sector = sno;
	}

	void set_opcode(uint8_t code) {
		opcode = code;
	}

	uint16_t sector_size() {
		return drives[current_drive].dtype.sector_size;
	}

	Drive & drive() {
		return drives[current_drive];
	}

	void set_dskfile_name(const char name[]) {
		strncpy(drives[current_drive].dskfname, name, 15);
		drives[current_drive].dskfname[15] = 0;
	}

	void setup_read(uint8_t drv, uint8_t trk, uint8_t sect) {
		current_drive = drv;
		drives[current_drive].track = trk;
		drives[current_drive].sector = sect;
		opcode = READ_SECTOR;
	}

	void setup_write(uint8_t drv, uint8_t trk, uint8_t sect) {
		current_drive = drv;
		drives[current_drive].track = trk;
		drives[current_drive].sector = sect;
		opcode = WRITE_SECTOR;
	}

	// calculate how many bytes from the file beginning
	// with respect to trk, sect, specified
	// track number begin with 0, sector number begin with 1 (to 27)
	uint64_t seek_position() {
		uint64_t sects = drive().track * drive().dtype.nof_sectors + (drive().sector - 1);
		return sects * drive().dtype.sector_size;
	}

	uint8_t status() { return RES_OK; }
};

#endif
