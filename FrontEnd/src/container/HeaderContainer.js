import Header from '../component/Header.js';
import { connect } from 'react-redux';
import {
  changePage,
  changeUser,
} from '../store/actions'

const mapStateToProps = state => ({
  page: state.page,
  username: state.username
})

const mapDispatchToProps = dispatch => ({
  onChangePage: page => dispatch(changePage(page)),
  onChangeUser: user => dispatch(changeUser(user)),
})

export default connect(mapStateToProps, mapDispatchToProps)(Header)
