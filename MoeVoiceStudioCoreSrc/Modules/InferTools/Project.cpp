#include "Project.hpp"

MoeVSProject::MoeVSProject::MoeVSProject(const std::wstring& _path)
{
    FILE* project_file = nullptr;
    _wfopen_s(&project_file, _path.c_str(), L"rb");
    if(!project_file)
        throw std::exception("File Doesn't Exists");
    fseek(project_file, 0, SEEK_SET);
    

    if (fread(&moevs_proj_header_, 1, sizeof(Header), project_file) != sizeof(Header))
        throw std::exception("Unexpected EOF");
    if (!(moevs_proj_header_.ChunkSymbol[0] == 'M' && moevs_proj_header_.ChunkSymbol[1] == 'O' && moevs_proj_header_.ChunkSymbol[2] == 'E' && moevs_proj_header_.ChunkSymbol[3] == 'V' && moevs_proj_header_.ChunkSymbol[4] == 'S' && moevs_proj_header_.ChunkSymbol[5] == 'P' && moevs_proj_header_.ChunkSymbol[6] == 'R' && moevs_proj_header_.ChunkSymbol[7] == 'J'))
        throw std::exception("Unrecognized File");
    if(moevs_proj_header_.DataHeaderAmount == 0)
        throw std::exception("Empty Project");


    data_pos_ = std::vector<size_type>(moevs_proj_header_.DataHeaderAmount);
    if (fread(data_pos_.data(), 1, sizeof(size_type) * (moevs_proj_header_.DataHeaderAmount), project_file) != sizeof(size_type) * (moevs_proj_header_.DataHeaderAmount))
        throw std::exception("Unexpected EOF");
    data_chunk_begin_ = sizeof(Header) + sizeof(size_type) * (moevs_proj_header_.DataHeaderAmount);


	size_type _n_bytes = 0;
    const size_type _data_begin = data_chunk_begin_;
    fseek(project_file, long(_data_begin), SEEK_SET);
    for(const auto& i : data_pos_)
    {
        if (i == data_pos_[moevs_proj_header_.DataHeaderAmount])
            break;
        Data _datas;


        //Header
        if (fread(&_datas.Header, 1, sizeof(DataHeader), project_file) != sizeof(DataHeader))
            throw std::exception("Unexpected EOF");
        if (!(_datas.Header.ChunkSymbol[0] == 'D' && _datas.Header.ChunkSymbol[1] == 'A' && _datas.Header.ChunkSymbol[2] == 'T' && _datas.Header.ChunkSymbol[3] == 'A'))
            throw std::exception("Unrecognized File");


        //HiddenUnit
        if (_datas.Header.HiddenUnitOffsetPosSize != 0) {
            _datas.Offset.Hidden_Unit = std::vector<size_type>(_datas.Header.HiddenUnitOffsetPosSize);
            _n_bytes = sizeof(size_type) * _datas.Header.HiddenUnitOffsetPosSize;
            if (fread(_datas.Offset.Hidden_Unit.data(), 1, _n_bytes, project_file) != _n_bytes)
                throw std::exception("Unexpected EOF");
            for (const auto& j : _datas.Offset.Hidden_Unit)
            {
                std::vector<float> hs_vector(j);
                _n_bytes = sizeof(float) * j;
                if (fread(hs_vector.data(), 1, _n_bytes, project_file) != _n_bytes)
                    throw std::exception("Unexpected EOF");
                _datas.ParamData.Hidden_Unit.emplace_back(std::move(hs_vector));
            }
        }


        //F0
        if (_datas.Header.F0OffsetPosSize != 0)
        {
            _datas.Offset.F0 = std::vector<size_type>(_datas.Header.F0OffsetPosSize);
            _n_bytes = sizeof(size_type) * _datas.Header.F0OffsetPosSize;
            if (fread(_datas.Offset.F0.data(), 1, _n_bytes, project_file) != _n_bytes)
                throw std::exception("Unexpected EOF");
            for (const auto& j : _datas.Offset.F0)
            {
                std::vector<float> f0_vector(j);
                _n_bytes = sizeof(float) * j;
                if (fread(f0_vector.data(), 1, _n_bytes, project_file) != _n_bytes)
                    throw std::exception("Unexpected EOF");
                _datas.ParamData.F0.emplace_back(std::move(f0_vector));
            }
        }


        //Volume
        if(_datas.Header.VolumeOffsetPosSize != 0)
        {
            _datas.Offset.Volume = std::vector<size_type>(_datas.Header.VolumeOffsetPosSize);
            _n_bytes = sizeof(size_type) * _datas.Header.VolumeOffsetPosSize;
            if (fread(_datas.Offset.Volume.data(), 1, _n_bytes, project_file) != _n_bytes)
                throw std::exception("Unexpected EOF");
            for (const auto& j : _datas.Offset.Volume)
            {
                std::vector<float> Volume_vector(j);
                _n_bytes = sizeof(float) * j;
                if (fread(Volume_vector.data(), 1, _n_bytes, project_file) != _n_bytes)
                    throw std::exception("Unexpected EOF");
                _datas.ParamData.Volume.emplace_back(std::move(Volume_vector));
            }
        }


        //ChrarcterMix
        if (_datas.Header.CharacterOffsetPosSize != 0)
        {
            _datas.Offset.Speaker = std::vector<size_type>(_datas.Header.CharacterOffsetPosSize);
            _n_bytes = sizeof(size_type) * _datas.Header.CharacterOffsetPosSize;
            if (fread(_datas.Offset.Speaker.data(), 1, _n_bytes, project_file) != _n_bytes)
                throw std::exception("Unexpected EOF");
            for (const auto& j : _datas.Offset.Speaker)
            {
                std::vector<float> Speaker_vector(j);
                _n_bytes = sizeof(float) * j;
                if (fread(Speaker_vector.data(), 1, _n_bytes, project_file) != _n_bytes)
                    throw std::exception("Unexpected EOF");
                _datas.ParamData.Speaker.emplace_back(std::move(Speaker_vector));
            }
        }


        //OrgLen
        if (_datas.Header.OrgLenSize != 0)
        {
	        _datas.ParamData.OrgLen = std::vector<long>(_datas.Header.OrgLenSize);
        	_n_bytes = sizeof(long) * _datas.Header.OrgLenSize;
        	if (fread(_datas.ParamData.OrgLen.data(), 1, _n_bytes, project_file) != _n_bytes)
        		throw std::exception("Unexpected EOF");
        }


        //Symbol
        if (_datas.Header.SymbolSize != 0)
        {
            std::vector<unsigned char> BooleanVector(_datas.Header.SymbolSize);
            _n_bytes = _datas.Header.SymbolSize;
            if (fread(BooleanVector.data(), 1, _n_bytes, project_file) != _n_bytes)
                throw std::exception("Unexpected EOF");
            _datas.ParamData.symbolb = std::vector<bool>((bool*)BooleanVector.data(), (bool*)(BooleanVector.data() + BooleanVector.size()));
        }


        //path
        if (_datas.Header.PathSize != 0)
        {
            std::vector<char> PathStr(_datas.Header.PathSize);
            _n_bytes = _datas.Header.PathSize;
            if (fread(PathStr.data(), 1, _n_bytes, project_file) != _n_bytes)
                throw std::exception("Unexpected EOF");
            _datas.ParamData.paths = to_wide_string(PathStr.data());
        }

        data_.emplace_back(std::move(_datas));
    }
    fclose(project_file);
}

MoeVSProject::MoeVSProject::MoeVSProject(const std::vector<Params>& _params, uint16_t HiddenSize)
{
    moevs_proj_header_.HiddenSize = HiddenSize;
    moevs_proj_header_.DataHeaderAmount = size_type(_params.size());
    data_chunk_begin_ = sizeof(Header) + sizeof(size_type) * (moevs_proj_header_.DataHeaderAmount);
    data_pos_.push_back(0);
    size_type _offset = 0;
    for(const auto& i : _params)
    {
        Data _data;
        _data.ParamData = i;

        for(const auto& hidden_unit : i.Hidden_Unit)
            _data.Offset.Hidden_Unit.push_back(hidden_unit.size());
        _data.Header.HiddenUnitOffsetPosSize = _data.Offset.Hidden_Unit.size();

        for (const auto& f0 : i.F0)
            _data.Offset.F0.push_back(f0.size());
        _data.Header.F0OffsetPosSize = _data.Offset.F0.size();

        if (!i.Volume.empty())
        {
            for (const auto& volume : i.Volume)
                _data.Offset.Volume.push_back(volume.size());
            _data.Header.VolumeOffsetPosSize = _data.Offset.Volume.size();
        }
        else
            _data.Header.VolumeOffsetPosSize = 0;

        if (!i.Speaker.empty())
        {
            for (const auto& Speaker : i.Speaker)
                _data.Offset.Speaker.push_back(Speaker.size());
            _data.Header.CharacterOffsetPosSize = _data.Offset.Speaker.size();
        }
        else
            _data.Header.CharacterOffsetPosSize = 0;

        _data.Header.OrgLenSize = i.OrgLen.size();

        _data.Header.SymbolSize = i.symbolb.size();

        const std::string bytePath = to_byte_string(i.paths);
        _data.Header.PathSize = bytePath.length() + 1;
        _offset += _data.Size();
        data_pos_.push_back(_offset);
        data_.emplace_back(std::move(_data));
    }

    data_pos_.pop_back();
}

void MoeVSProject::MoeVSProject::Write(const std::wstring& _path) const
{
    FILE* project_file = nullptr;
    _wfopen_s(&project_file, _path.c_str(), L"wb");
    if (!project_file)
        throw std::exception("Cannot Create File");

    fwrite(&moevs_proj_header_, 1, sizeof(Header), project_file);
    fwrite(data_pos_.data(), 1, data_pos_.size() * sizeof(size_type), project_file);

    for (const auto& i : data_)
    {
        fwrite(&i.Header, 1, sizeof(DataHeader), project_file);


        fwrite(i.Offset.Hidden_Unit.data(), 1, sizeof(size_type) * i.Offset.Hidden_Unit.size(), project_file);
        for (const auto& hidden_unit : i.ParamData.Hidden_Unit)
            fwrite(hidden_unit.data(), 1, hidden_unit.size() * sizeof(float), project_file);


        fwrite(i.Offset.F0.data(), 1, sizeof(size_type) * i.Offset.F0.size(), project_file);
        for (const auto& f0 : i.ParamData.F0)
            fwrite(f0.data(), 1, f0.size() * sizeof(float), project_file);


        if(i.Header.VolumeOffsetPosSize != 0)
        {
            fwrite(i.Offset.Volume.data(), 1, sizeof(size_type) * i.Offset.Volume.size(), project_file);
            for (const auto& volume : i.ParamData.Volume)
                fwrite(volume.data(), 1, volume.size() * sizeof(float), project_file);
        }

        if (i.Header.CharacterOffsetPosSize != 0)
        {
            fwrite(i.Offset.Speaker.data(), 1, sizeof(size_type) * i.Offset.Speaker.size(), project_file);
            for (const auto& Speaker : i.ParamData.Speaker)
                fwrite(Speaker.data(), 1, Speaker.size() * sizeof(float), project_file);
        }

        fwrite(i.ParamData.OrgLen.data(), 1, sizeof(long) * i.ParamData.OrgLen.size(), project_file);

        std::vector<char> BooleanVector(i.ParamData.symbolb.size());
        for (size_t index = 0; index < i.ParamData.symbolb.size(); ++index)
            BooleanVector[index] = i.ParamData.symbolb[index] ? 1 : 0;
        fwrite(BooleanVector.data(), 1, i.ParamData.symbolb.size(), project_file);

        const std::string bytePath = to_byte_string(i.ParamData.paths);
        fwrite(bytePath.data(), 1, i.Header.PathSize, project_file);
    }
    fclose(project_file);
}
