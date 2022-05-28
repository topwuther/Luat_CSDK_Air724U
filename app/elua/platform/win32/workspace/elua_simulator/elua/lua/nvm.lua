
--配置数据管理
module(...,package.seeall)
local FILENAME="/set.ini"

local default = {
	installer = "012345",
	alarmnum = {"","","","",""},
	opcode = "1234",
	admin = {"","",""},
	alarmode = {1,1,1,1,1},
	callrepeat = 5,
	ctrler = {0x10000,0x10000,0x10000,0x10000},
	wdef = {0x10000,0x10000,0x10000,0x10000,0x10000},
	alarmtext = {"您的汽车车门正被人非法打开，请赶紧处理",
				"您的汽车尾箱正被人非法打开，请赶紧处理",
				"您的汽车或油箱正被人撬砸，请赶紧处理",
				"您的汽车油箱正被人打开，请赶紧处理",
				"您的汽车电池正被人拆除，请赶紧处理",
				[99]="您的汽车电池断电，请赶紧处理",},
	alertdelaytime = 20000, -- 全布防延时报警
	incallinterval = 20000,-- 来电间隔
	incalltimes = 2, -- 来电达到次数后翻转
	delayautoalert = 30000,-- 撤防后延时等待时间自动布防
	def2valid = "开通", -- 是否屏蔽防区2
	def4stat = "开路", -- 有线防区4触发状态
	def5stat = "开路", -- 有线防区5触发状态
}

local paras = default

local function readval(t,vtype,k,v)
	if vtype == "string" then
		t[k] = v
	elseif vtype == "number" and tonumber(v) then
		t[k] = tonumber(v)
	elseif vtype == "boolean" then
		t[k] = v == "true" and true or false
	else
		print("loadata: unknwon",vtype,k,v)
	end
end

local function writeval(v)
	local outs = ""

	if type(v) == "table" then print("invalid type table") return end

	outs = tostring(v)

	return outs
end

local function save()
	local f = io.open(FILENAME,"wb")

	if f == nil then
		print("save:open file error")
		return
	end

	local s,vtype

	for k,v in pairs(paras) do
		vtype = type(v)

		f:write(k .. "=")
		if vtype == "table" then
			f:write(table.concat(v,","))
		else
			s = writeval(v)
			if s ~= nil then
				f:write(s)
			end
		end
		f:write("\n")
	end
	f:close()
end

local function loadata()
	local f = io.open(FILENAME)

	if f == nil then
		save()
		return
	end

	local key,val,vtype

	for s in f:lines() do
		key,val = string.match(s,"(%w+)=(.*)$")

		if key and val and default[key] then
			vtype = type(default[key])
			if vtype == "table" then
				local tmax = #default[key]
				local idx = 1
				for vv in string.gmatch(val,"([^,]+)") do
					readval(paras[key],type(default[key][idx]),idx,vv)
					idx = idx+1
					if idx >= tmax then
						break
					end
				end
			else
				readval(paras,vtype,key,val)
			end
		end
	end
	f:close()
end

function restore()
	paras = default
	save()
end

local function setvalue(k,v)
	if paras[k] ~= v then
		paras[k] = v
		save()
	end
end

local function settvalue(k,idx,v)
	if paras[k][idx] ~= v then
		paras[k][idx] = v
		save()
	end
end

local function checkset(k,p1,p2)
	if default[k] == nil then print("unknwon element", k) return end

	if type(default[k]) == "table" then

		if default[k][p1] == nil then
			print("invalid index", k, p1)
			return
		end

		if p2 == nil then return end

		if type(p2) ~= type(default[k][p1]) then print("type error",type(p2)) return end

		if paras[k][p1] == p2 then return end

		return 1
	else
		if type(p1) ~= type(default[k]) then print("type error",type(p1)) return end

		if paras[k] == p1 then return end
	end

	return 0
end

function set(k,p1,p2)
	local ret = checkset(k,p1,p2)

	if ret == 1 then settvalue(k,p1,p2)
	elseif ret == 0 then setvalue(k,p1)
	end
end

function get(k,p1)
	if default[k] == nil then print("unknwon element",k) return end

	if p1 == nil then return paras[k] end

	return paras[k][p1]
end

function isadmin(num)
	for k,v in ipairs(paras.admin) do
		if v == num then
			return true
		end
	end
end

function validctrler(id)
	for i,v in ipairs(paras.ctrler) do
		if v == id then
			return true
		end
	end
end

function isalarmnum(num)
	for k,v in ipairs(paras.alarmnum) do
		if v == num then
			return true
		end
	end
end

function checkwdef(id)
	for k,v in ipairs(paras.wdef) do
		if v == id then
			return k
		end
	end
end

loadata()
